#pragma once

#include <string_view>
#include <memory>

namespace mtn {

	class Texture2D {
	public:
		Texture2D() = default;
		Texture2D(std::string_view path);
		Texture2D(std::shared_ptr<uint32_t[]> data, uint32_t width, uint32_t height, uint8_t numChannels = 4);
		Texture2D(uint32_t width, uint32_t height, uint8_t numChannels = 4);
		~Texture2D();
		Texture2D(Texture2D&& tex2d) noexcept; // move constructor
		Texture2D(const Texture2D& tex2d); // copy operator

		Texture2D& operator=(Texture2D&& tex2d) noexcept; // move operator
		Texture2D& operator=(const Texture2D& tex2d); // copy constructor

		void setData();
		void setData(std::shared_ptr<uint32_t[]> data);
		bool resize(uint32_t width, uint32_t height);

		inline std::shared_ptr<uint32_t[]> getData() const { return pData_; }
		inline uint32_t getWidth() const { return width_; }
		inline uint32_t getHeight() const { return height_; }
		inline uint8_t getNumChannels() const { return numChannels_; }

		inline unsigned int getId() const { return id_; }

		void bind(const unsigned int& texUnit) const;
		void unbind() const;

	private:
		void initTexture();
		void setStorage();

		unsigned int id_;
		mutable bool isBound_;
		mutable unsigned int boundTexUnit_;

		std::shared_ptr<uint32_t[]> pData_;
		uint32_t width_, height_;
		uint8_t numChannels_;
	};

}