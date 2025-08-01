// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: partner_battle_data.proto

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "partner_battle_data.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace {

const ::google::protobuf::Descriptor* PB_PartnerBattleData_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  PB_PartnerBattleData_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_partner_5fbattle_5fdata_2eproto() {
  protobuf_AddDesc_partner_5fbattle_5fdata_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "partner_battle_data.proto");
  GOOGLE_CHECK(file != NULL);
  PB_PartnerBattleData_descriptor_ = file->message_type(0);
  static const int PB_PartnerBattleData_offsets_[7] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(PB_PartnerBattleData, battle_data_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(PB_PartnerBattleData, skin_id_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(PB_PartnerBattleData, battle_row_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(PB_PartnerBattleData, battle_column_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(PB_PartnerBattleData, other_sp_num_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(PB_PartnerBattleData, other_sp_list_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(PB_PartnerBattleData, str_lv_),
  };
  PB_PartnerBattleData_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      PB_PartnerBattleData_descriptor_,
      PB_PartnerBattleData::default_instance_,
      PB_PartnerBattleData_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(PB_PartnerBattleData, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(PB_PartnerBattleData, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(PB_PartnerBattleData));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_partner_5fbattle_5fdata_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    PB_PartnerBattleData_descriptor_, &PB_PartnerBattleData::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_partner_5fbattle_5fdata_2eproto() {
  delete PB_PartnerBattleData::default_instance_;
  delete PB_PartnerBattleData_reflection_;
}

void protobuf_AddDesc_partner_5fbattle_5fdata_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::protobuf_AddDesc_battle_5fcharacter_5fdata_2eproto();
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\031partner_battle_data.proto\032\033battle_char"
    "acter_data.proto\"\301\001\n\024PB_PartnerBattleDat"
    "a\022,\n\013battle_data\030\001 \001(\0132\027.PB_BattleCharac"
    "terData\022\017\n\007skin_id\030\002 \001(\005\022\022\n\nbattle_row\030\003"
    " \001(\005\022\031\n\rbattle_column\030\004 \001(\005:\002-1\022\024\n\014other"
    "_sp_num\030\005 \001(\005\022\025\n\rother_sp_list\030\006 \003(\005\022\016\n\006"
    "str_lv\030\007 \001(\005", 252);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "partner_battle_data.proto", &protobuf_RegisterTypes);
  PB_PartnerBattleData::default_instance_ = new PB_PartnerBattleData();
  PB_PartnerBattleData::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_partner_5fbattle_5fdata_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_partner_5fbattle_5fdata_2eproto {
  StaticDescriptorInitializer_partner_5fbattle_5fdata_2eproto() {
    protobuf_AddDesc_partner_5fbattle_5fdata_2eproto();
  }
} static_descriptor_initializer_partner_5fbattle_5fdata_2eproto_;

// ===================================================================

#ifndef _MSC_VER
const int PB_PartnerBattleData::kBattleDataFieldNumber;
const int PB_PartnerBattleData::kSkinIdFieldNumber;
const int PB_PartnerBattleData::kBattleRowFieldNumber;
const int PB_PartnerBattleData::kBattleColumnFieldNumber;
const int PB_PartnerBattleData::kOtherSpNumFieldNumber;
const int PB_PartnerBattleData::kOtherSpListFieldNumber;
const int PB_PartnerBattleData::kStrLvFieldNumber;
#endif  // !_MSC_VER

PB_PartnerBattleData::PB_PartnerBattleData()
  : ::google::protobuf::Message() {
  SharedCtor();
  // @@protoc_insertion_point(constructor:PB_PartnerBattleData)
}

void PB_PartnerBattleData::InitAsDefaultInstance() {
  battle_data_ = const_cast< ::PB_BattleCharacterData*>(&::PB_BattleCharacterData::default_instance());
}

PB_PartnerBattleData::PB_PartnerBattleData(const PB_PartnerBattleData& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
  // @@protoc_insertion_point(copy_constructor:PB_PartnerBattleData)
}

void PB_PartnerBattleData::SharedCtor() {
  _cached_size_ = 0;
  battle_data_ = NULL;
  skin_id_ = 0;
  battle_row_ = 0;
  battle_column_ = -1;
  other_sp_num_ = 0;
  str_lv_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

PB_PartnerBattleData::~PB_PartnerBattleData() {
  // @@protoc_insertion_point(destructor:PB_PartnerBattleData)
  SharedDtor();
}

void PB_PartnerBattleData::SharedDtor() {
  if (this != default_instance_) {
    delete battle_data_;
  }
}

void PB_PartnerBattleData::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* PB_PartnerBattleData::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return PB_PartnerBattleData_descriptor_;
}

const PB_PartnerBattleData& PB_PartnerBattleData::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_partner_5fbattle_5fdata_2eproto();
  return *default_instance_;
}

PB_PartnerBattleData* PB_PartnerBattleData::default_instance_ = NULL;

PB_PartnerBattleData* PB_PartnerBattleData::New() const {
  return new PB_PartnerBattleData;
}

void PB_PartnerBattleData::Clear() {
#define OFFSET_OF_FIELD_(f) (reinterpret_cast<char*>(      \
  &reinterpret_cast<PB_PartnerBattleData*>(16)->f) - \
   reinterpret_cast<char*>(16))

#define ZR_(first, last) do {                              \
    size_t f = OFFSET_OF_FIELD_(first);                    \
    size_t n = OFFSET_OF_FIELD_(last) - f + sizeof(last);  \
    ::memset(&first, 0, n);                                \
  } while (0)

  if (_has_bits_[0 / 32] & 95) {
    ZR_(skin_id_, battle_row_);
    if (has_battle_data()) {
      if (battle_data_ != NULL) battle_data_->::PB_BattleCharacterData::Clear();
    }
    battle_column_ = -1;
    other_sp_num_ = 0;
    str_lv_ = 0;
  }

#undef OFFSET_OF_FIELD_
#undef ZR_

  other_sp_list_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool PB_PartnerBattleData::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:PB_PartnerBattleData)
  for (;;) {
    ::std::pair< ::google::protobuf::uint32, bool> p = input->ReadTagWithCutoff(127);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional .PB_BattleCharacterData battle_data = 1;
      case 1: {
        if (tag == 10) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_battle_data()));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(16)) goto parse_skin_id;
        break;
      }

      // optional int32 skin_id = 2;
      case 2: {
        if (tag == 16) {
         parse_skin_id:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &skin_id_)));
          set_has_skin_id();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(24)) goto parse_battle_row;
        break;
      }

      // optional int32 battle_row = 3;
      case 3: {
        if (tag == 24) {
         parse_battle_row:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &battle_row_)));
          set_has_battle_row();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(32)) goto parse_battle_column;
        break;
      }

      // optional int32 battle_column = 4 [default = -1];
      case 4: {
        if (tag == 32) {
         parse_battle_column:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &battle_column_)));
          set_has_battle_column();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(40)) goto parse_other_sp_num;
        break;
      }

      // optional int32 other_sp_num = 5;
      case 5: {
        if (tag == 40) {
         parse_other_sp_num:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &other_sp_num_)));
          set_has_other_sp_num();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(48)) goto parse_other_sp_list;
        break;
      }

      // repeated int32 other_sp_list = 6;
      case 6: {
        if (tag == 48) {
         parse_other_sp_list:
          DO_((::google::protobuf::internal::WireFormatLite::ReadRepeatedPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 1, 48, input, this->mutable_other_sp_list())));
        } else if (tag == 50) {
          DO_((::google::protobuf::internal::WireFormatLite::ReadPackedPrimitiveNoInline<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, this->mutable_other_sp_list())));
        } else {
          goto handle_unusual;
        }
        if (input->ExpectTag(48)) goto parse_other_sp_list;
        if (input->ExpectTag(56)) goto parse_str_lv;
        break;
      }

      // optional int32 str_lv = 7;
      case 7: {
        if (tag == 56) {
         parse_str_lv:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &str_lv_)));
          set_has_str_lv();
        } else {
          goto handle_unusual;
        }
        if (input->ExpectAtEnd()) goto success;
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0 ||
            ::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:PB_PartnerBattleData)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:PB_PartnerBattleData)
  return false;
#undef DO_
}

void PB_PartnerBattleData::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:PB_PartnerBattleData)
  // optional .PB_BattleCharacterData battle_data = 1;
  if (has_battle_data()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      1, this->battle_data(), output);
  }

  // optional int32 skin_id = 2;
  if (has_skin_id()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(2, this->skin_id(), output);
  }

  // optional int32 battle_row = 3;
  if (has_battle_row()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(3, this->battle_row(), output);
  }

  // optional int32 battle_column = 4 [default = -1];
  if (has_battle_column()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(4, this->battle_column(), output);
  }

  // optional int32 other_sp_num = 5;
  if (has_other_sp_num()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(5, this->other_sp_num(), output);
  }

  // repeated int32 other_sp_list = 6;
  for (int i = 0; i < this->other_sp_list_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(
      6, this->other_sp_list(i), output);
  }

  // optional int32 str_lv = 7;
  if (has_str_lv()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(7, this->str_lv(), output);
  }

  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:PB_PartnerBattleData)
}

::google::protobuf::uint8* PB_PartnerBattleData::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // @@protoc_insertion_point(serialize_to_array_start:PB_PartnerBattleData)
  // optional .PB_BattleCharacterData battle_data = 1;
  if (has_battle_data()) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        1, this->battle_data(), target);
  }

  // optional int32 skin_id = 2;
  if (has_skin_id()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(2, this->skin_id(), target);
  }

  // optional int32 battle_row = 3;
  if (has_battle_row()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(3, this->battle_row(), target);
  }

  // optional int32 battle_column = 4 [default = -1];
  if (has_battle_column()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(4, this->battle_column(), target);
  }

  // optional int32 other_sp_num = 5;
  if (has_other_sp_num()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(5, this->other_sp_num(), target);
  }

  // repeated int32 other_sp_list = 6;
  for (int i = 0; i < this->other_sp_list_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteInt32ToArray(6, this->other_sp_list(i), target);
  }

  // optional int32 str_lv = 7;
  if (has_str_lv()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(7, this->str_lv(), target);
  }

  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:PB_PartnerBattleData)
  return target;
}

int PB_PartnerBattleData::ByteSize() const {
  int total_size = 0;

  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional .PB_BattleCharacterData battle_data = 1;
    if (has_battle_data()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
          this->battle_data());
    }

    // optional int32 skin_id = 2;
    if (has_skin_id()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->skin_id());
    }

    // optional int32 battle_row = 3;
    if (has_battle_row()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->battle_row());
    }

    // optional int32 battle_column = 4 [default = -1];
    if (has_battle_column()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->battle_column());
    }

    // optional int32 other_sp_num = 5;
    if (has_other_sp_num()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->other_sp_num());
    }

    // optional int32 str_lv = 7;
    if (has_str_lv()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->str_lv());
    }

  }
  // repeated int32 other_sp_list = 6;
  {
    int data_size = 0;
    for (int i = 0; i < this->other_sp_list_size(); i++) {
      data_size += ::google::protobuf::internal::WireFormatLite::
        Int32Size(this->other_sp_list(i));
    }
    total_size += 1 * this->other_sp_list_size() + data_size;
  }

  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void PB_PartnerBattleData::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const PB_PartnerBattleData* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const PB_PartnerBattleData*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void PB_PartnerBattleData::MergeFrom(const PB_PartnerBattleData& from) {
  GOOGLE_CHECK_NE(&from, this);
  other_sp_list_.MergeFrom(from.other_sp_list_);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_battle_data()) {
      mutable_battle_data()->::PB_BattleCharacterData::MergeFrom(from.battle_data());
    }
    if (from.has_skin_id()) {
      set_skin_id(from.skin_id());
    }
    if (from.has_battle_row()) {
      set_battle_row(from.battle_row());
    }
    if (from.has_battle_column()) {
      set_battle_column(from.battle_column());
    }
    if (from.has_other_sp_num()) {
      set_other_sp_num(from.other_sp_num());
    }
    if (from.has_str_lv()) {
      set_str_lv(from.str_lv());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void PB_PartnerBattleData::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void PB_PartnerBattleData::CopyFrom(const PB_PartnerBattleData& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool PB_PartnerBattleData::IsInitialized() const {

  return true;
}

void PB_PartnerBattleData::Swap(PB_PartnerBattleData* other) {
  if (other != this) {
    std::swap(battle_data_, other->battle_data_);
    std::swap(skin_id_, other->skin_id_);
    std::swap(battle_row_, other->battle_row_);
    std::swap(battle_column_, other->battle_column_);
    std::swap(other_sp_num_, other->other_sp_num_);
    other_sp_list_.Swap(&other->other_sp_list_);
    std::swap(str_lv_, other->str_lv_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata PB_PartnerBattleData::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = PB_PartnerBattleData_descriptor_;
  metadata.reflection = PB_PartnerBattleData_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

// @@protoc_insertion_point(global_scope)
