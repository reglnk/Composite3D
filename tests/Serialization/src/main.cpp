
#include <bitsery/bitsery.h>
#include <bitsery/details/serialization_common.h>
#include <bitsery/adapter/buffer.h>
#include <bitsery/ext/std_smart_ptr.h>
#include <bitsery/traits/vector.h>

#include <cm3d/Tl/DynArray.hpp>
#include <cm3d/Tl/UniqueStorage.hpp>
#include <cm3d/Serialization/Types/DynArray.hpp>

#include <cm3d/Core/FileSystem.hpp>

#include <iostream>
#include <fstream>
#include <memory>

void expect(bool smth) {
	if (!smth)
		throw "Assertion failed!";
	// std::iterator_traits<cm3d::dynArrForwardIterator<int>> ui;
	// std::iterator_traits<std::vector<int>::const_iterator>::iterator_category cat;
	// std::vector<int>::const_iterator::iterator_category catt;
}

struct MyStruct
{
	cm3d::DynArray<cm3d::DynArray<int>> ty;
	cm3d::DynArray<cm3d::DynArray<float>> fs;
	cm3d::UniqueStorage<std::shared_ptr<float>> us;
};

template<typename TOutputAdapter, typename TContext>
void serialize(bitsery::Serializer<TOutputAdapter, TContext>& s, MyStruct const& o)
{
	cm3d::sSize vecsize = o.ty.size();
	s.value8b(vecsize);
	for (auto const &inner: o.ty)
		s.container4b(inner, 1000);
	
	vecsize = o.fs.size();
	s.value8b(vecsize);
	for (auto const &inner: o.fs)
		s.container4b(inner, 1000);
	
	s.object(o.us);
}

template<typename TInputAdapter, typename TContext>
void serialize(bitsery::Deserializer<TInputAdapter, TContext>& s, MyStruct& o)
{
	cm3d::sSize vecsize;
	s.value8b(vecsize);
	o.ty.resize(vecsize);
	for (auto &inner: o.ty)
		s.container4b(inner, 1000);
	
	s.value8b(vecsize);
	o.fs.resize(vecsize);
	for (auto &inner: o.fs)
		s.container4b(inner, 1000);
	
	s.object(o.us);
}

// template<typename T>
// void serialize(T& s, MyStruct& o)
// {
// 	std::cout << "serialization\n";
// 	s.container4b(o.ty
	
// 	// s.object(o.us);
// }

// constexpr bool hasfn = bitsery::details::HasSerializeMethod <
// 	bitsery::Serializer <
// 		bitsery::OutputBufferAdapter<std::vector<unsigned char, std::allocator<unsigned char>>, bitsery::DefaultConfig>,
// 		std::tuple<bitsery::ext::PointerLinkingContext>
// 	>,
// 	// myshared<float>,
// 	cm3d::Serialization::shared_ptr<float>,
// 	// MyStruct,
// 	void
// >::value;

// some helper types
using Buffer = std::vector<uint8_t>;
using OutputAdapter = bitsery::OutputBufferAdapter<Buffer>;
using InputAdapter = bitsery::InputBufferAdapter<Buffer>;

using TContext = std::tuple<bitsery::ext::PointerLinkingContext>;
using MySerializer = bitsery::Serializer<OutputAdapter, TContext>;
using MyDeserializer = bitsery::Deserializer<InputAdapter, TContext>;

int main(int argc, char **argv)
{
	if (argc < 2) {
		std::cout << "Usage: " << argv[0] << " <filename> [r | w]\n";
		return 1;
	}
	bool writeMode = argc >= 3 && !strcmp(argv[2], "w");
	MyStruct data;
	
	Buffer buffer;
	
	if (writeMode)
	{
		std::cout << "Enter integers (-1 to end): ";
		int input;
		for (;;) {
			std::cin >> input;
			if (input == -1)
				break;

			cm3d::DynArray<int> p;
			for (int i = 0; i != 10; ++i) p.pushBack(input + i);
			data.ty.pushBack(p);
		}
		
		std::cout << "Enter floats (0.0 to end): ";
		float inputf;
		for (;;) {
			std::cin >> inputf;
			if (inputf == 0.f)
				break;

			cm3d::DynArray<float> p;
			for (int i = 0; i != 10; ++i) p.pushBack(inputf + i);
			data.fs.pushBack(p);
		}
		
		std::cout << "Enter floats (0.0 to end, negative to skip): ";
		for (;;) {
			std::cin >> inputf;
			if (inputf == 0.f)
				break;

			data.us.push(std::make_shared<float>(inputf));
		}
		assert(data.us.size() == data.us.length());
		for (size_t i = 0; i != data.us.size(); ++i) {
			if (data.us[i].get()[0] < 0.f) {
				data.us.erase(i);
			}
		}

		TContext ctx{};
		MySerializer ser{ ctx, buffer };
		ser.object(data);
		ser.adapter().flush();
		auto writtenSize = ser.adapter().writtenBytesCount();
		
		cm3d::FileSystem::writeFile(argv[1], buffer.data(), writtenSize);
	}
	else
	{
		cm3d::sSize fsize;
		if (cm3d::FileSystem::getFileSize(argv[1], &fsize)) {
			std::cout << "Failed to get file size\n";
			return -1;
		}
		
		buffer.resize(fsize);
		if (cm3d::FileSystem::readFile(argv[1], buffer.data(), fsize)) {
			std::cout << "Failed to read file\n";
			return -1;
		}
		
		TContext ctx{};
		size_t writtenSize = fsize;
		MyDeserializer des{ ctx, buffer.begin(), writtenSize };
		des.object(data);
		
		assert(std::get<0>(ctx).isValid());
		std::get<0>(ctx).clearSharedState();

		if (des.adapter().error() != bitsery::ReaderError::NoError || !des.adapter().isCompletedSuccessfully()) {
			std::cout << "Failed to deserialize data\n";
			return -1;
		}
		
		std::cout << "ty:\n";
		for (auto const &arr: data.ty) {
			for (auto const &val: arr) {
				std::cout << val << ", ";
			}
			std::cout << '\n';
		}
		std::cout << "fs:\n";
		for (auto const &arr: data.fs) {
			for (auto const &val: arr) {
				std::cout << val << ", ";
			}
			std::cout << '\n';
		}
		std::cout << "us:\n";
		for (auto iter = data.us.begin(); iter != data.us.end(); ++iter)
			std::cout << iter.index() << " | " << (((*iter).get()) ? ((*iter).get()[0]) : (1.f / 0.f)) << '\n';
		
		std::cout << std::endl;
	}
}
