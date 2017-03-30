# Copyright 2016-2017 CodiLime
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

import socket

import msgpack

from veles.proto import messages, msgpackwrap
from veles.proto.exceptions import VelesException
from veles.schema import nodeid


class Client:
    def __init__(self, sock):
        self.sock = sock
        wrapper = msgpackwrap.MsgpackWrapper()
        self.unpacker = wrapper.unpacker
        self.packer = wrapper.packer

    def getpkt(self):
        while True:
            try:
                return messages.MsgpackMsg.load(self.unpacker.unpack())
            except msgpack.OutOfData:
                pass
            data = self.sock.recv(1024)
            if not data:
                raise Exception("end of file")
            self.unpacker.feed(data)

    def send_msg(self, msg):
        self.sock.sendall(self.packer.pack(msg.dump()))

    def request(self, msg):
        self.send_msg(msg)
        pkt = self.getpkt()
        if isinstance(pkt, messages.MsgRequestAck) and pkt.rid == 0:
            return msg.id
        elif isinstance(pkt, messages.MsgRequestError) and pkt.rid == 0:
            raise VelesException(pkt.code, pkt.msg)
        else:
            print(pkt)
            raise Exception('weird reply to request')

    def create(self, parent, *, tags=set(), attr={}, data={}, bindata={},
               pos=(None, None)):
        msg = messages.MsgCreate(
            id=nodeid.NodeID(),
            parent=parent,
            pos_start=pos[0],
            pos_end=pos[1],
            tags=tags,
            attr=attr,
            data=data,
            bindata=bindata,
            rid=0,
        )
        self.request(msg)
        return msg.id

    def delete(self, obj):
        msg = messages.MsgDelete(
            id=obj,
            rid=0
        )
        self.request(msg)

    def set_parent(self, obj, parent):
        msg = messages.MsgSetParent(
            id=obj,
            parent=parent,
            rid=0
        )
        self.request(msg)

    def set_pos(self, obj, start, end):
        msg = messages.MsgSetPos(
            id=obj,
            pos_start=start,
            pos_end=end,
            rid=0
        )
        self.request(msg)

    def add_tag(self, obj, tag):
        msg = messages.MsgAddTag(
            id=obj,
            tag=tag,
            rid=0
        )
        self.request(msg)

    def del_tag(self, obj, tag):
        msg = messages.MsgDelTag(
            id=obj,
            tag=tag,
            rid=0
        )
        self.request(msg)

    def set_attr(self, obj, key, data):
        msg = messages.MsgSetAttr(
            id=obj,
            key=key,
            data=data,
            rid=0
        )
        self.request(msg)

    def set_data(self, obj, key, data):
        msg = messages.MsgSetData(
            id=obj,
            rid=0,
            key=key,
            data=data,
        )
        self.request(msg)

    def set_bindata(self, obj, key, start, data, truncate=False):
        msg = messages.MsgSetBinData(
            id=obj,
            rid=0,
            key=key,
            start=start,
            data=data,
            truncate=truncate,
        )
        self.request(msg)

    def get(self, obj):
        msg = messages.MsgGet(
            id=obj,
            qid=0,
        )
        self.send_msg(msg)
        pkt = self.getpkt()
        if isinstance(pkt, messages.MsgGetReply) and pkt.qid == 0:
            return pkt.obj
        elif isinstance(pkt, messages.MsgQueryError) and pkt.qid == 0:
            raise VelesException(pkt.code, pkt.msg)
        else:
            raise Exception('weird reply to get')

    def get_sub(self, obj):
        msg = messages.MsgGet(
            id=obj,
            qid=0,
            sub=True,
        )
        self.send_msg(msg)
        while True:
            pkt = self.getpkt()
            if isinstance(pkt, messages.MsgGetReply) and pkt.qid == 0:
                yield pkt.obj
            elif isinstance(pkt, messages.MsgQueryError) and pkt.qid == 0:
                raise VelesException(pkt.code, pkt.msg)
            else:
                raise Exception('weird reply to get')

    def get_data(self, obj, key):
        msg = messages.MsgGetData(
            id=obj,
            qid=0,
            key=key,
        )
        self.send_msg(msg)
        pkt = self.getpkt()
        if isinstance(pkt, messages.MsgGetDataReply) and pkt.qid == 0:
            return pkt.data
        elif isinstance(pkt, messages.MsgQueryError) and pkt.qid == 0:
            raise VelesException(pkt.code, pkt.msg)
        else:
            raise Exception('weird reply to get_data')

    def get_data_sub(self, obj, key):
        msg = messages.MsgGetData(
            id=obj,
            qid=0,
            key=key,
            sub=True
        )
        self.send_msg(msg)
        while True:
            pkt = self.getpkt()
            if isinstance(pkt, messages.MsgGetDataReply) and pkt.qid == 0:
                yield pkt.data
            elif isinstance(pkt, messages.MsgQueryError) and pkt.qid == 0:
                raise VelesException(pkt.code, pkt.msg)
            else:
                raise Exception('weird reply to get_data')

    def get_bindata(self, obj, key, start=0, end=None):
        msg = messages.MsgGetBinData(
            id=obj,
            qid=0,
            key=key,
            start=start,
            end=end,
        )
        self.send_msg(msg)
        pkt = self.getpkt()
        if isinstance(pkt, messages.MsgGetBinDataReply) and pkt.qid == 0:
            return pkt.data
        elif isinstance(pkt, messages.MsgQueryError) and pkt.qid == 0:
            raise VelesException(pkt.code, pkt.msg)
        else:
            raise Exception('weird reply to get_bindata')

    def get_bindata_sub(self, obj, key, start=0, end=None):
        msg = messages.MsgGetBinData(
            id=obj,
            qid=0,
            key=key,
            start=start,
            end=end,
            sub=True,
        )
        self.send_msg(msg)
        while True:
            pkt = self.getpkt()
            if isinstance(pkt, messages.MsgGetBinDataReply) and pkt.qid == 0:
                yield pkt.data
            elif isinstance(pkt, messages.MsgQueryError) and pkt.qid == 0:
                raise VelesException(pkt.code, pkt.msg)
            else:
                raise Exception('weird reply to get_bindata')

    def list(self, obj):
        msg = messages.MsgGetList(
            qid=0,
            parent=obj,
        )
        self.send_msg(msg)
        pkt = self.getpkt()
        if isinstance(pkt, messages.MsgGetListReply) and pkt.qid == 0:
            return pkt.objs
        elif isinstance(pkt, messages.MsgQueryError) and pkt.qid == 0:
            raise VelesException(pkt.code, pkt.msg)
        else:
            print(pkt)
            raise Exception('weird reply to list')

    def list_sub(self, obj):
        msg = messages.MsgGetList(
            qid=0,
            parent=obj,
            sub=True
        )
        self.send_msg(msg)
        while True:
            pkt = self.getpkt()
            if isinstance(pkt, messages.MsgGetListReply) and pkt.qid == 0:
                yield pkt
            elif isinstance(pkt, messages.MsgQueryError) and pkt.qid == 0:
                raise VelesException(pkt.code, pkt.msg)
            else:
                print(pkt)
                raise Exception('weird reply to list')


class UnixClient(Client):
    def __init__(self, path):
        sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        sock.connect(path)
        super().__init__(sock)


class TcpClient(Client):
    def __init__(self, ip, port):
        sock = socket.create_connection((ip, port))
        super().__init__(sock)


def create_client(addr):
    host, _, port = addr.rpartition(':')
    if host == 'UNIX':
        return UnixClient(port)
    else:
        return TcpClient(host, int(port))
