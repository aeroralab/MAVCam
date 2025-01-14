// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: mavcam_options.proto

#include "mavcam_options.pb.h"

#include <algorithm>
#include "google/protobuf/io/coded_stream.h"
#include "google/protobuf/extension_set.h"
#include "google/protobuf/wire_format_lite.h"
#include "google/protobuf/descriptor.h"
#include "google/protobuf/generated_message_reflection.h"
#include "google/protobuf/reflection_ops.h"
#include "google/protobuf/wire_format.h"
#include "google/protobuf/generated_message_tctable_impl.h"
// @@protoc_insertion_point(includes)

// Must be included last.
#include "google/protobuf/port_def.inc"
PROTOBUF_PRAGMA_INIT_SEG
namespace _pb = ::google::protobuf;
namespace _pbi = ::google::protobuf::internal;
namespace _fl = ::google::protobuf::internal::field_layout;
namespace mavcam {
namespace options {
}  // namespace options
}  // namespace mavcam
static const ::_pb::EnumDescriptor* file_level_enum_descriptors_mavcam_5foptions_2eproto[1];
static constexpr const ::_pb::ServiceDescriptor**
    file_level_service_descriptors_mavcam_5foptions_2eproto = nullptr;
const ::uint32_t TableStruct_mavcam_5foptions_2eproto::offsets[1] = {};
static constexpr ::_pbi::MigrationSchema* schemas = nullptr;
static constexpr ::_pb::Message* const* file_default_instances = nullptr;
const char descriptor_table_protodef_mavcam_5foptions_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
    "\n\024mavcam_options.proto\022\016mavcam.options\032 "
    "google/protobuf/descriptor.proto**\n\tAsyn"
    "cType\022\t\n\005ASYNC\020\000\022\010\n\004SYNC\020\001\022\010\n\004BOTH\020\002:6\n\r"
    "default_value\022\035.google.protobuf.FieldOpt"
    "ions\030\320\206\003 \001(\t:0\n\007epsilon\022\035.google.protobu"
    "f.FieldOptions\030\321\206\003 \001(\001:O\n\nasync_type\022\036.g"
    "oogle.protobuf.MethodOptions\030\320\206\003 \001(\0162\031.m"
    "avcam.options.AsyncType:3\n\tis_finite\022\036.g"
    "oogle.protobuf.MethodOptions\030\321\206\003 \001(\010B\020\n\016"
    "options.mavcamb\006proto3"
};
static const ::_pbi::DescriptorTable* const descriptor_table_mavcam_5foptions_2eproto_deps[1] =
    {
        &::descriptor_table_google_2fprotobuf_2fdescriptor_2eproto,
};
static ::absl::once_flag descriptor_table_mavcam_5foptions_2eproto_once;
const ::_pbi::DescriptorTable descriptor_table_mavcam_5foptions_2eproto = {
    false,
    false,
    382,
    descriptor_table_protodef_mavcam_5foptions_2eproto,
    "mavcam_options.proto",
    &descriptor_table_mavcam_5foptions_2eproto_once,
    descriptor_table_mavcam_5foptions_2eproto_deps,
    1,
    0,
    schemas,
    file_default_instances,
    TableStruct_mavcam_5foptions_2eproto::offsets,
    nullptr,
    file_level_enum_descriptors_mavcam_5foptions_2eproto,
    file_level_service_descriptors_mavcam_5foptions_2eproto,
};

// This function exists to be marked as weak.
// It can significantly speed up compilation by breaking up LLVM's SCC
// in the .pb.cc translation units. Large translation units see a
// reduction of more than 35% of walltime for optimized builds. Without
// the weak attribute all the messages in the file, including all the
// vtables and everything they use become part of the same SCC through
// a cycle like:
// GetMetadata -> descriptor table -> default instances ->
//   vtables -> GetMetadata
// By adding a weak function here we break the connection from the
// individual vtables back into the descriptor table.
PROTOBUF_ATTRIBUTE_WEAK const ::_pbi::DescriptorTable* descriptor_table_mavcam_5foptions_2eproto_getter() {
  return &descriptor_table_mavcam_5foptions_2eproto;
}
// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2
static ::_pbi::AddDescriptorsRunner dynamic_init_dummy_mavcam_5foptions_2eproto(&descriptor_table_mavcam_5foptions_2eproto);
namespace mavcam {
namespace options {
const ::google::protobuf::EnumDescriptor* AsyncType_descriptor() {
  ::google::protobuf::internal::AssignDescriptors(&descriptor_table_mavcam_5foptions_2eproto);
  return file_level_enum_descriptors_mavcam_5foptions_2eproto[0];
}
PROTOBUF_CONSTINIT const uint32_t AsyncType_internal_data_[] = {
    196608u, 0u, };
bool AsyncType_IsValid(int value) {
  return 0 <= value && value <= 2;
}
const std::string default_value_default("");
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2 ::google::protobuf::internal::ExtensionIdentifier< ::google::protobuf::FieldOptions,
    ::google::protobuf::internal::StringTypeTraits, 9, false>
  default_value(kDefaultValueFieldNumber, default_value_default, nullptr);
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2 ::google::protobuf::internal::ExtensionIdentifier< ::google::protobuf::FieldOptions,
    ::google::protobuf::internal::PrimitiveTypeTraits< double >, 1, false>
  epsilon(kEpsilonFieldNumber, 0, nullptr);
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2 ::google::protobuf::internal::ExtensionIdentifier< ::google::protobuf::MethodOptions,
    ::google::protobuf::internal::EnumTypeTraits< ::mavcam::options::AsyncType, ::mavcam::options::AsyncType_IsValid>, 14, false>
  async_type(kAsyncTypeFieldNumber, static_cast< ::mavcam::options::AsyncType >(0), nullptr);
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2 ::google::protobuf::internal::ExtensionIdentifier< ::google::protobuf::MethodOptions,
    ::google::protobuf::internal::PrimitiveTypeTraits< bool >, 8, false>
  is_finite(kIsFiniteFieldNumber, false, nullptr);
// @@protoc_insertion_point(namespace_scope)
}  // namespace options
}  // namespace mavcam
namespace google {
namespace protobuf {
}  // namespace protobuf
}  // namespace google
// @@protoc_insertion_point(global_scope)
#include "google/protobuf/port_undef.inc"
