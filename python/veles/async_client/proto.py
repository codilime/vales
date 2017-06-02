# Copyright 2017 CodiLime
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

# Through the darkness of future past
# The magician longs to see
# One chants out between two worlds
# Fire, walk with me

import asyncio
import logging
import ssl

import msgpack

from veles.proto import messages, msgpackwrap
from veles.proto.exceptions import VelesException, SchemaError
from veles.proto.messages import PROTO_VERSION
from veles.util import helpers

logger = logging.getLogger('veles.async_client')


class ClientProto(asyncio.Protocol):
    def __init__(self, auth_key, fingerprint='', name='acli',
                 version='1.0', description='', type='acli'):
        wrapper = msgpackwrap.MsgpackWrapper()
        self.unpacker = wrapper.unpacker
        self.packer = wrapper.packer
        self.qids = {}
        self.mids = {}
        self.bids = {}
        self.rids = {}
        self.handlers = {}
        self.auth_key = helpers.prepare_auth_key(auth_key)
        self.client_name = name
        self.client_version = version
        self.client_description = description
        self.client_type = type
        self.fingerprint = fingerprint

    def _authorize(self):
        self.transport.write(self.auth_key)
        self.send_msg(messages.MsgConnect(
            proto_version=PROTO_VERSION,
            client_name=self.client_name,
            client_version=self.client_version,
            client_description=self.client_description,
            client_type=self.client_type,
        ))

    def connection_made(self, transport):
        cert = transport.get_extra_info('ssl_object').getpeercert(True)
        helpers.validate_cert(cert, self.fingerprint)
        self.transport = transport
        self._authorize()

    def data_received(self, data):
        self.unpacker.feed(data)
        while True:
            try:
                msg = messages.MsgpackMsg.load(self.unpacker.unpack())
                loop = asyncio.get_event_loop()
                loop.create_task(self.handle_msg(msg))
            except msgpack.OutOfData:
                return

    async def handle_msg(self, msg):
        handlers = {
            'connection_error': self.msg_connection_error,
            'connected': self.msg_connected,
            'get_reply': self.msg_get_reply,
            'get_data_reply': self.msg_get_reply,
            'get_bindata_reply': self.msg_get_reply,
            'get_list_reply': self.msg_get_reply,
            'get_query_reply': self.msg_get_reply,
            'connections_reply': self.msg_get_reply,
            'query_error': self.msg_query_error,
            'subscription_cancelled': self.msg_subscription_cancelled,
            'proto_error': self.msg_proto_error,
            'request_ack': self.msg_request_ack,
            'request_error': self.msg_request_error,
            'method_result': self.msg_method_result,
            'method_error': self.msg_method_error,
            'broadcast_result': self.msg_broadcast_result,
            'plugin_method_run': self.msg_plugin_method_run,
            'plugin_query_get': self.msg_plugin_query_get,
            'plugin_broadcast_run': self.msg_plugin_broadcast_run,
            'plugin_trigger_run': self.msg_plugin_trigger_run,
            'plugin_handler_unregistered':
                self.msg_plugin_handler_unregistered,
        }
        if msg.object_type not in handlers:
            raise SchemaError('unhandled message type: {}'.format(
                msg.object_type))
        await handlers[msg.object_type](msg)

    def connection_lost(self, ex):
        raise RuntimeError("Connection lost")

    def send_msg(self, msg):
        self.transport.write(self.packer.pack(msg.dump()))

    async def msg_get_reply(self, msg):
        self.qids[msg.qid].handle_reply(msg)

    async def msg_query_error(self, msg):
        self.qids[msg.qid].handle_error(msg.err, msg.checks)

    async def msg_subscription_cancelled(self, msg):
        del self.qids[msg.qid]

    async def msg_request_ack(self, msg):
        self.rids[msg.rid].handle_ack()

    async def msg_request_error(self, msg):
        self.rids[msg.rid].handle_error(msg.err)

    async def msg_method_result(self, msg):
        self.mids[msg.mid].handle_result(msg.result)

    async def msg_method_error(self, msg):
        self.mids[msg.mid].handle_error(msg.err)

    async def msg_broadcast_result(self, msg):
        self.bids[msg.bid].handle_result(msg.results)

    async def msg_proto_error(self, msg):
        raise msg.err

    async def msg_connection_error(self, msg):
        raise msg.err

    async def msg_connected(self, msg):
        logger.info('Connected to server: {}'.format(msg.server_name))

    async def msg_plugin_method_run(self, msg):
        handler = self.handlers[msg.phid]
        aresult = handler.run_method(self.conn, msg.node, msg.params)
        try:
            result = await aresult
        except VelesException as e:
            self.send_msg(messages.MsgPluginMethodError(
                pmid=msg.pmid,
                err=e,
            ))
        else:
            self.send_msg(messages.MsgPluginMethodResult(
                pmid=msg.pmid,
                result=result
            ))

    async def msg_plugin_query_get(self, msg):
        handler = self.handlers[msg.phid]
        checks = []
        aresult = handler.get_query(self.conn, msg.node, msg.params, checks)
        try:
            result = await aresult
        except VelesException as e:
            self.send_msg(messages.MsgPluginQueryError(
                pqid=msg.pqid,
                err=e,
                checks=checks,
            ))
        else:
            self.send_msg(messages.MsgPluginQueryResult(
                pqid=msg.pqid,
                result=result,
                checks=checks,
            ))

    async def msg_plugin_broadcast_run(self, msg):
        handler = self.handlers[msg.phid]
        aresults = handler.run_broadcast(self.conn, msg.params)
        results = await aresults
        self.send_msg(messages.MsgPluginBroadcastResult(
            pbid=msg.pbid,
            results=results
        ))

    async def msg_plugin_trigger_run(self, msg):
        handler = self.handlers[msg.phid]
        checks = []
        aresult = handler.run_trigger(self.conn, msg.node, checks)
        try:
            await aresult
        except VelesException as e:
            self.send_msg(messages.MsgPluginTriggerError(
                ptid=msg.ptid,
                err=e,
                checks=checks,
            ))
        else:
            self.send_msg(messages.MsgPluginTriggerDone(
                ptid=msg.ptid,
                checks=checks,
            ))

    async def msg_plugin_handler_unregistered(self, msg):
        del self.handlers[msg.phid]


async def create_client(loop, url):
    url = helpers.parse_url(url)
    if url.scheme == helpers.UrlScheme.UNIX_SCHEME:
        return await loop.create_unix_connection(
            lambda: ClientProto(url.auth_key, fingerprint=url.fingerprint),
            url.path)
    elif url.scheme == helpers.UrlScheme.TCP_SCHEME:
        return await loop.create_connection(
            lambda: ClientProto(url.auth_key, fingerprint=url.fingerprint),
            url.host, url.port)
    elif url.scheme == helpers.UrlScheme.SSL_SCHEME:
        return await loop.create_connection(
            lambda: ClientProto(url.auth_key, fingerprint=url.fingerprint),
            url.host, url.port,
            ssl=ssl.SSLContext())
    else:
        raise ValueError('Wrong scheme provided!')
