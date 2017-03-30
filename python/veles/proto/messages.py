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

from veles.proto.node import Node, PosFilter
from veles.schema import model, fields
from veles.schema.nodeid import NodeID


class MsgpackMsg(model.PolymorphicModel):
    pass


# XXX NYI
class MsgConnect(MsgpackMsg):
    """
    Sent by the client immediately upon connection.  Server replies with
    MsgConnected or MsgConnectionError.
    """

    object_type = 'connect'

    proto_version = fields.SmallInteger(minimum=1)
    client_name = fields.String(optional=True)
    client_version = fields.String(optional=True)
    client_description = fields.String(optional=True)
    client_type = fields.String(optional=True)
    key = fields.String(optional=True)


# XXX NYI
class MsgConnected(MsgpackMsg):
    """
    Sent by the server in reply to MsgConnect.
    """

    object_type = 'connected'

    proto_version = fields.SmallInteger(minimum=1)
    server_name = fields.String()
    server_version = fields.String()


# XXX NYI
class MsgConnectionError(MsgpackMsg):
    """
    Sent by the server in reply to MsgConnect.
    """

    object_type = 'connection_error'

    code = fields.String()
    msg = fields.String()


class MsgProtoError(MsgpackMsg):
    """
    Sent by the server in reply to a malformed packet.
    """

    object_type = 'proto_error'

    code = fields.String()
    msg = fields.String()


# XXX NYI
class MsgRegisterMethod(MsgpackMsg):
    object_type = 'register_mthd'


# XXX NYI
class MsgRegisterTrigger(MsgpackMsg):
    object_type = 'register_trigger'


# queries and subscriptions


class MsgGet(MsgpackMsg):
    """
    Sent by the client to request information about the given node.
    If sub is True, a subscription is established - the server will send
    a MsgGetReply (or MsgQueryError) immediately, then send it again
    every time any part of the reply changes, until a MsgCancelSubscription
    is received.  If sub is False, only one reply will be sent.
    The replies are matched with queries via ``qid``, which are assigned
    by the client.  It's an error to send a query with a ``qid`` that
    is already in use for an active subscription.
    """

    object_type = 'get'

    qid = fields.SmallUnsignedInteger()
    id = fields.NodeID()
    sub = fields.Boolean(default=False)


class MsgGetReply(MsgpackMsg):
    """
    Sent by server in reply to MsgGet.
    """

    object_type = 'get_reply'

    qid = fields.SmallUnsignedInteger()
    obj = fields.Object(Node)


class MsgGetData(MsgpackMsg):
    """
    Sent by client, requests the data associated with the given node and key.
    Works like MsgGet, but replies with MsgGetData.  It is not an error if
    a key doesn't exist - None will be returned in this case.
    """

    object_type = 'get_data'

    qid = fields.SmallUnsignedInteger()
    id = fields.NodeID()
    key = fields.String()
    sub = fields.Boolean(default=False)


class MsgGetDataReply(MsgpackMsg):
    """
    Sent by server in reply to MsgGetData.
    """

    object_type = 'get_data_reply'

    qid = fields.SmallUnsignedInteger()
    data = fields.Any(optional=True)


class MsgGetBinData(MsgpackMsg):
    """
    Sent by client, requests a range of bindata associated with the given node
    and key.  Works like MsgGet, but replies with MsgGetBinData.  The range
    is left-inclusive and right-exclusive.  It is not an error if the key
    doesn't exist - empty bytestring will be returned in this case.
    It is also not an error if the range is out of bounds for the bindata
    - it will be truncated if it's partially in range, or an empty bytestring
    will be returned if it's completely out of range.
    """

    object_type = 'get_bindata'

    qid = fields.SmallUnsignedInteger()
    id = fields.NodeID()
    key = fields.String()
    start = fields.SmallUnsignedInteger()
    end = fields.SmallUnsignedInteger(optional=True)
    sub = fields.Boolean(default=False)


class MsgGetBinDataReply(MsgpackMsg):
    """
    Sent by server in reply to MsgGetBinData.
    """

    object_type = 'get_bindata_reply'

    qid = fields.SmallUnsignedInteger()
    data = fields.Binary()


class MsgGetList(MsgpackMsg):
    """
    Sent by client, requests a list of children of the given node matching
    the given filters.  Children can be filtered by tags (the set of tags
    in the query must be a subset of the node's tags for it to match)
    and by a position filter.

    If sub is False, server will reply with a single MsgGetListReply with
    gone=[], or with a single MsgQueryError.  If sub is True, the first
    reply will be like for sub=False, but subsequent replies will only
    mention changed nodes - nodes that are new on the list, along with
    changed nodes remaining on the list are sent in the objs field
    of the reply, while the IDs of nodes no longer on the list are sent
    in the gone field of the reply.

    The list may fail with ObjectGoneErro if the parent node is gone.
    If it reappears in the future, a MsgGetListReply will be sent
    with a complete list of children.
    """

    object_type = 'get_list'

    qid = fields.SmallUnsignedInteger()
    parent = fields.NodeID(default=NodeID.root_id)
    tags = fields.Set(fields.String())
    pos_filter = fields.Object(PosFilter, default=PosFilter())
    sub = fields.Boolean(default=False)


class MsgGetListReply(MsgpackMsg):
    """
    Sent by server in reply to MsgGetList.
    """

    object_type = 'get_list_reply'

    qid = fields.SmallUnsignedInteger()
    objs = fields.List(fields.Object(Node))
    gone = fields.List(fields.NodeID())


# XXX NYI
class MsgListConnections(MsgpackMsg):
    object_type = 'list_connections'


# XXX NYI
class MsgConnectionsReply(MsgpackMsg):
    object_type = 'connections_reply'


# XXX NYI
class MsgListRegistry(MsgpackMsg):
    object_type = 'list_registry'


# XXX NYI
class MsgRegistryReply(MsgpackMsg):
    object_type = 'registry_reply'


class MsgQueryError(MsgpackMsg):
    """
    Sent by the server in reply to MsgGet*.  Note that receiving this doesn't
    kill the subscription - a successful reply may be sent later if
    the situation improves.
    """

    object_type = 'query_error'

    qid = fields.SmallUnsignedInteger()
    code = fields.String()
    msg = fields.String()


class MsgCancelSubscription(MsgpackMsg):
    """
    Sent by the client.  Cancels a subscription with the given qid.
    The qid must refer to a previously established subscription.
    Server replies with MsgSubscriptionCancelled - once that is received,
    no more replies to the subscription will be coming, and it's safe
    to reuse its qid.
    """

    object_type = 'cancel_subscription'

    qid = fields.SmallUnsignedInteger()


class MsgSubscriptionCancelled(MsgpackMsg):
    """
    Sent by the server in reply to MsgCancelSubscription.
    """

    object_type = 'subscription_cancelled'

    qid = fields.SmallUnsignedInteger()


# mutators

class MsgCreate(MsgpackMsg):
    """
    Creates a node on the server.  It is an error if a node with given id
    already exists.  Server replies with MsgRequestAck or MsgRequestError.
    Replies are matched to requests by ``rid``, which is assigned by the
    client.
    """

    object_type = 'create'

    rid = fields.SmallUnsignedInteger()
    id = fields.NodeID()
    parent = fields.NodeID(default=NodeID.root_id)
    pos_start = fields.Integer(optional=True)
    pos_end = fields.Integer(optional=True)
    tags = fields.Set(fields.String())
    attr = fields.Map(fields.String(), fields.Any())
    data = fields.Map(fields.String(), fields.Any())
    bindata = fields.Map(fields.String(), fields.Binary())


class MsgDelete(MsgpackMsg):
    """
    Deletes the node with a given ID, along with all of its children
    (recursively).  It is not an error if the node doesn't exist
    (nothing is done in this case).  Server replies with MsgRequestAck
    or MsgRequestError.
    """

    object_type = 'delete'

    rid = fields.SmallUnsignedInteger()
    id = fields.NodeID()


class MsgSetParent(MsgpackMsg):
    """
    Changes the parent of a given node.  It is an error if new parent
    is a child of the given node.  Server replies with MsgRequestAck
    or MsgRequestError.
    """

    object_type = 'set_parent'

    rid = fields.SmallUnsignedInteger()
    id = fields.NodeID()
    parent = fields.NodeID(default=NodeID.root_id)


class MsgSetPos(MsgpackMsg):
    """
    Changes the position of a given node.  Server replies with MsgRequestAck
    or MsgRequestError.
    """

    object_type = 'set_pos'

    rid = fields.SmallUnsignedInteger()
    id = fields.NodeID()
    pos_start = fields.Integer(optional=True)
    pos_end = fields.Integer(optional=True)


class MsgAddTag(MsgpackMsg):
    """
    Adds a tag to a given node.  Server replies with MsgRequestAck
    or MsgRequestError.  If the node already has the given tag, nothing
    happens.
    """

    object_type = 'add_tag'

    rid = fields.SmallUnsignedInteger()
    id = fields.NodeID()
    tag = fields.String()


class MsgDelTag(MsgpackMsg):
    """
    Removes a tag from a given node.  Server replies with MsgRequestAck
    or MsgRequestError.  If the node doesn't have the given tag, nothing
    happens.
    """

    object_type = 'del_tag'

    rid = fields.SmallUnsignedInteger()
    id = fields.NodeID()
    tag = fields.String()


class MsgSetAttr(MsgpackMsg):
    """
    Sets an attribute on a given node.  Server replies with MsgRequestAck
    or MsgRequestError.  Setting an attribute to None is equivalent to
    deleting it.

    """

    object_type = 'set_attr'

    rid = fields.SmallUnsignedInteger()
    id = fields.NodeID()
    key = fields.String()
    data = fields.Any(optional=True)


class MsgSetData(MsgpackMsg):
    """
    Sets a data value on a given node.  Server replies with MsgRequestAck
    or MsgRequestError.  Setting a data value to None is equivalent to
    deleting it.

    """

    object_type = 'set_data'

    rid = fields.SmallUnsignedInteger()
    id = fields.NodeID()
    key = fields.String()
    data = fields.Any(optional=True)


class MsgSetBinData(MsgpackMsg):
    """
    Sets a range of bindata on a given node.  Server replies with MsgRequestAck
    or MsgRequestError.  The bindata is modified starting from a given start
    position - it is an error if the position is after the current end of
    bindata (but not if it's equal).  The bindata is expanded if necessary
    to hold the new data.  If truncate is set, the bindata is truncated
    after the end of the new data.  If bindata would become 0-length,
    it is deleted.
    """

    object_type = 'set_bindata'

    rid = fields.SmallUnsignedInteger()
    id = fields.NodeID()
    key = fields.String()
    start = fields.SmallUnsignedInteger(default=0)
    data = fields.Binary()
    truncate = fields.Boolean(default=False)


class MsgRequestAck(MsgpackMsg):
    """
    Sent by the server in reply to modification requests.
    """

    object_type = 'request_ack'

    rid = fields.SmallUnsignedInteger()


class MsgRequestError(MsgpackMsg):
    """
    Sent by the server in reply to modification requests.
    """

    object_type = 'request_error'

    rid = fields.SmallUnsignedInteger()
    code = fields.String()
    msg = fields.String()


# methods & triggers

# XXX NYI
class MsgMethodRun(MsgpackMsg):
    object_type = 'mthd_run'


# XXX NYI
class MsgMethodRes(MsgpackMsg):
    object_type = 'mthd_res'


# XXX NYI
class MsgMethodError(MsgpackMsg):
    object_type = 'mthd_err'


# XXX NYI
class MsgPluginMethodRun(MsgpackMsg):
    object_type = 'plugin_mthd_run'


# XXX NYI
class MsgPluginMethodDone(MsgpackMsg):
    object_type = 'plugin_mthd_done'


# XXX NYI
class MsgPluginMethodError(MsgpackMsg):
    object_type = 'plugin_mthd_err'


# XXX NYI
class MsgPluginTriggerRun(MsgpackMsg):
    object_type = 'plugin_trigger_run'


# XXX NYI
class MsgPluginTriggerDone(MsgpackMsg):
    object_type = 'plugin_trigger_done'


# XXX NYI
class MsgPluginTriggerError(MsgpackMsg):
    object_type = 'plugin_trigger_err'
