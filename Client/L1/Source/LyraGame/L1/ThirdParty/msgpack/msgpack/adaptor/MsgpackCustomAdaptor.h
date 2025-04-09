// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "variant"
#include "vector"
#include "msgpack.hpp"

namespace msgpack {
	MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
		namespace adaptor {

			template<typename T>
			struct convert<TArray<T>> {
				msgpack::object const& operator()(msgpack::object const& o, TArray<T>& v) const {
					if (o.type != msgpack::type::ARRAY) {
						UE_LOG(LogTemp, Error, TEXT("TArray<T> convert failed: expected array but got type %d"), (int)o.type);
						throw msgpack::type_error();
					}

					v.Empty();
					for (size_t i = 0; i < o.via.array.size; ++i) {
						T item;
						try {
							o.via.array.ptr[i].convert(item);
						}
						catch (const msgpack::type_error& e) {
							UE_LOG(LogTemp, Error, TEXT("Type error: %s"), *FString(e.what()));
							UE_LOG(LogTemp, Error, TEXT("TArray<T>: failed to convert element at index %d: type = %d"), (int)i, (int)o.via.array.ptr[i].type);
							throw;
						}
						v.Add(item);
					}
					return o;
				}
			};

			template<typename T>
			struct pack<TArray<T>> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, const TArray<T>& v) const {
					o.pack_array(v.Num());
					for (const auto& item : v) {
						o.pack(item);
					}
					return o;
				}
			};

			template<typename T>
			struct object_with_zone<TArray<T>> {
				void operator()(msgpack::object::with_zone& o, const TArray<T>& v) const {
					o.type = msgpack::type::ARRAY;
					o.via.array.size = v.Num();
					o.via.array.ptr = static_cast<msgpack::object*>(
						o.zone.allocate_align(sizeof(msgpack::object) * v.Num())
						);

					for (size_t i = 0; i < v.Num(); ++i) {
						o.via.array.ptr[i] = msgpack::object(v[i], o.zone);
					}
				}
			};

			template <typename... Types>
			struct pack<std::variant<Types...>> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, const std::variant<Types...>& v) const {
					std::visit([&o](auto&& value) { o.pack(value); }, v);
					return o;
				}
			};
			template <typename... Types>
			struct object_with_zone<std::variant<Types...>> {
				void operator()(msgpack::object::with_zone& o, const std::variant<Types...>& v) const {
					std::visit([&o](auto&& value) { o << value; }, v);
				}
			};

			template<>
			struct convert<FString> {
				msgpack::object const& operator()(msgpack::object const& o, FString& v) const {
					// MsgPack 데이터를 std::string으로 읽고 FString으로 변환
					if (o.type == msgpack::type::STR) {
						v = UTF8_TO_TCHAR(o.as<std::string>().c_str());
					}
					else {
						throw msgpack::type_error();
					}
					return o;
				}
			};

			// FString에 대한 직렬화 규칙
			template<>
			struct pack<FString> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, const FString& v) const {
					// FString을 std::string으로 변환하여 MsgPack에 저장
					std::string utf8String = TCHAR_TO_UTF8(*v);
					o.pack(utf8String);
					return o;
				}
			};

			// FString에 대한 객체 규칙 정의
			template<>
			struct object_with_zone<FString> {
				void operator()(msgpack::object::with_zone& o, const FString& v) const {
					// FString을 std::string으로 변환 후 MsgPack 객체로 변환
					std::string utf8String = TCHAR_TO_UTF8(*v);
					o.type = msgpack::type::STR;
					o.via.str.size = utf8String.size();
					o.via.str.ptr = utf8String.c_str();
				}
			};

			template<>
			struct convert<int64> {
				msgpack::object const& operator()(msgpack::object const& o, int64& v) const {
					if (o.type == msgpack::type::POSITIVE_INTEGER) {
						v = static_cast<int64>(o.via.u64);
					}
					else if (o.type == msgpack::type::NEGATIVE_INTEGER) {
						v = static_cast<int64>(o.via.i64);
					}
					else {
						throw msgpack::type_error();
					}
					return o;
				}
			};

			template<>
			struct pack<int64> {
				template <typename Stream>
				packer<Stream>& operator()(msgpack::packer<Stream>& o, const int64& v) const {
					o.pack_int64(v);
					return o;
				}
			};

			template<>
			struct object_with_zone<int64> {
				void operator()(msgpack::object::with_zone& o, const int64& v) const {
					o.type = msgpack::type::NEGATIVE_INTEGER;
					o.via.i64 = v;
				}
			};


		}
	}
}