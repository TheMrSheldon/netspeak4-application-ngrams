// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: NetspeakService.proto

#include "NetspeakService.pb.h"
#include "NetspeakService.grpc.pb.h"

#include <functional>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/client_callback.h>
#include <grpcpp/impl/codegen/message_allocator.h>
#include <grpcpp/impl/codegen/method_handler.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/server_callback.h>
#include <grpcpp/impl/codegen/server_callback_handlers.h>
#include <grpcpp/impl/codegen/server_context.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace netspeak {
namespace service {

static const char* NetspeakService_method_names[] = {
  "/netspeak.service.NetspeakService/Search",
  "/netspeak.service.NetspeakService/GetCorpora",
};

std::unique_ptr< NetspeakService::Stub> NetspeakService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< NetspeakService::Stub> stub(new NetspeakService::Stub(channel));
  return stub;
}

NetspeakService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_Search_(NetspeakService_method_names[0], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_GetCorpora_(NetspeakService_method_names[1], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status NetspeakService::Stub::Search(::grpc::ClientContext* context, const ::netspeak::service::SearchRequest& request, ::netspeak::service::SearchResponse* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_Search_, context, request, response);
}

void NetspeakService::Stub::experimental_async::Search(::grpc::ClientContext* context, const ::netspeak::service::SearchRequest* request, ::netspeak::service::SearchResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Search_, context, request, response, std::move(f));
}

void NetspeakService::Stub::experimental_async::Search(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::netspeak::service::SearchResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_Search_, context, request, response, std::move(f));
}

void NetspeakService::Stub::experimental_async::Search(::grpc::ClientContext* context, const ::netspeak::service::SearchRequest* request, ::netspeak::service::SearchResponse* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_Search_, context, request, response, reactor);
}

void NetspeakService::Stub::experimental_async::Search(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::netspeak::service::SearchResponse* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_Search_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::netspeak::service::SearchResponse>* NetspeakService::Stub::AsyncSearchRaw(::grpc::ClientContext* context, const ::netspeak::service::SearchRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::netspeak::service::SearchResponse>::Create(channel_.get(), cq, rpcmethod_Search_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::netspeak::service::SearchResponse>* NetspeakService::Stub::PrepareAsyncSearchRaw(::grpc::ClientContext* context, const ::netspeak::service::SearchRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::netspeak::service::SearchResponse>::Create(channel_.get(), cq, rpcmethod_Search_, context, request, false);
}

::grpc::Status NetspeakService::Stub::GetCorpora(::grpc::ClientContext* context, const ::netspeak::service::CorporaRequest& request, ::netspeak::service::CorporaResponse* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_GetCorpora_, context, request, response);
}

void NetspeakService::Stub::experimental_async::GetCorpora(::grpc::ClientContext* context, const ::netspeak::service::CorporaRequest* request, ::netspeak::service::CorporaResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_GetCorpora_, context, request, response, std::move(f));
}

void NetspeakService::Stub::experimental_async::GetCorpora(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::netspeak::service::CorporaResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc_impl::internal::CallbackUnaryCall(stub_->channel_.get(), stub_->rpcmethod_GetCorpora_, context, request, response, std::move(f));
}

void NetspeakService::Stub::experimental_async::GetCorpora(::grpc::ClientContext* context, const ::netspeak::service::CorporaRequest* request, ::netspeak::service::CorporaResponse* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_GetCorpora_, context, request, response, reactor);
}

void NetspeakService::Stub::experimental_async::GetCorpora(::grpc::ClientContext* context, const ::grpc::ByteBuffer* request, ::netspeak::service::CorporaResponse* response, ::grpc::experimental::ClientUnaryReactor* reactor) {
  ::grpc_impl::internal::ClientCallbackUnaryFactory::Create(stub_->channel_.get(), stub_->rpcmethod_GetCorpora_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::netspeak::service::CorporaResponse>* NetspeakService::Stub::AsyncGetCorporaRaw(::grpc::ClientContext* context, const ::netspeak::service::CorporaRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::netspeak::service::CorporaResponse>::Create(channel_.get(), cq, rpcmethod_GetCorpora_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::netspeak::service::CorporaResponse>* NetspeakService::Stub::PrepareAsyncGetCorporaRaw(::grpc::ClientContext* context, const ::netspeak::service::CorporaRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc_impl::internal::ClientAsyncResponseReaderFactory< ::netspeak::service::CorporaResponse>::Create(channel_.get(), cq, rpcmethod_GetCorpora_, context, request, false);
}

NetspeakService::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      NetspeakService_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< NetspeakService::Service, ::netspeak::service::SearchRequest, ::netspeak::service::SearchResponse>(
          std::mem_fn(&NetspeakService::Service::Search), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      NetspeakService_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< NetspeakService::Service, ::netspeak::service::CorporaRequest, ::netspeak::service::CorporaResponse>(
          std::mem_fn(&NetspeakService::Service::GetCorpora), this)));
}

NetspeakService::Service::~Service() {
}

::grpc::Status NetspeakService::Service::Search(::grpc::ServerContext* context, const ::netspeak::service::SearchRequest* request, ::netspeak::service::SearchResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status NetspeakService::Service::GetCorpora(::grpc::ServerContext* context, const ::netspeak::service::CorporaRequest* request, ::netspeak::service::CorporaResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace netspeak
}  // namespace service

