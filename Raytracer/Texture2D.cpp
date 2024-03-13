#include "Texture2D.h"

#include "Logger.h"

#include <glad.h>

namespace mtn {

	Texture2D::Texture2D(std::shared_ptr<uint32_t[]> data, uint32_t width, uint32_t height, uint8_t numChannels)
		: boundTexUnit_(-1), isBound_(false) {

		Logger::trace("Texture2D(uint32_t* const)");

		initTexture();
		setStorage();

		pData_ = data;
		width_ = static_cast<uint32_t>(width);
		height_ = static_cast<uint32_t>(height);
		numChannels_ = static_cast<uint8_t>(numChannels);

		setData();

		//stbi_image_free(reinterpret_cast<stbi_uc*>(pData_));
	}

	// This constructor is for creating texture storage to be supplied data later on
	Texture2D::Texture2D(uint32_t width, uint32_t height, uint8_t numChannels)
		: boundTexUnit_(-1), isBound_(false), width_(width), height_(height), numChannels_(numChannels) {

		Logger::trace("Texture2D(uint32_t, uint32_t)");

		initTexture();
		setStorage();
		pData_ = std::shared_ptr<uint32_t[]>(new uint32_t[width * height]);
	}

	void Texture2D::setData() {
		Logger::trace("Texture2D::setData()");

		if (pData_) {
			glTextureSubImage2D(id_, 0, 0, 0, width_, height_, GL_RGBA, GL_UNSIGNED_BYTE, pData_.get());
		}
		else {
			Logger::error("Failed to load 2D texture data.");
		}
	}

	void Texture2D::setData(std::shared_ptr<uint32_t[]> data) {
		Logger::trace("Texture2D::setData(uint32_t*)");

		pData_ = data;
		setData();
	}

	bool Texture2D::resize(uint32_t width, uint32_t height) {
		Logger::trace("Texture2D::onResize(uint32_t, uint32_t)");

		if (width == width_ && height == height_) {
			return false;
		}

		glDeleteTextures(1, &id_);

		width_ = width;
		height_ = height;

		initTexture();
		
		pData_ = std::shared_ptr<uint32_t[]>(new uint32_t[width * height]);

		setStorage();

		return true;
	}

	void Texture2D::bind(const unsigned int& texUnit) const {
		Logger::trace("Texture2D::bind(const unsigned int&)");

		boundTexUnit_ = texUnit;
		isBound_ = true;
		glBindTextureUnit(texUnit, id_);
	}

	void Texture2D::unbind() const {
		Logger::trace("Texture2D::unbind()");

		if (!isBound_) {
			Logger::debug("Texture {} is not bound to any texture unit. It cannot be unbound.", id_);
			return;
		}

		glBindTextureUnit(boundTexUnit_, 0);
		boundTexUnit_ = -1;
		isBound_ = false;
	}

	void Texture2D::initTexture() {
		Logger::trace("Texture2D::initTexture()");

		glCreateTextures(GL_TEXTURE_2D, 1, &id_);
		//glGenTextures(1, &id_);
		//bind(0);

		// TODO: Have user provide a sampler for this
		glTextureParameteri(id_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(id_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTextureParameteri(id_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTextureParameteri(id_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}

	void Texture2D::setStorage() {
		Logger::trace("Texture2D::setStorage()"); 

		glTextureStorage2D(id_, 1, GL_RGBA8, width_, height_);
	}

	// Copy constructor
	Texture2D::Texture2D(const Texture2D& tex2d)
		: id_(tex2d.id_),
		  boundTexUnit_(tex2d.boundTexUnit_),
		  pData_(tex2d.pData_),
		  width_(tex2d.width_),
		  height_(tex2d.height_),
		  numChannels_(tex2d.numChannels_),
		  isBound_(tex2d.isBound_) {

		Logger::trace("Texture2D(const Texture2D&)");
	}

	// Copy operator
	Texture2D& Texture2D::operator=(const Texture2D& tex2d) {
		Logger::trace("Texture2D::operator=(const Texture2D&)");

		if (this != &tex2d) {
			id_ = tex2d.id_,
			boundTexUnit_ = tex2d.boundTexUnit_;
			pData_ = tex2d.pData_,
			width_ = tex2d.width_;
			height_ = tex2d.height_;
			numChannels_ = tex2d.numChannels_;
			isBound_ = tex2d.isBound_;
		}

		return *this;
	}

	// Move constructor
	Texture2D::Texture2D(Texture2D&& tex2d) noexcept 
		: id_(std::move(tex2d.id_)),
		  boundTexUnit_(std::move(tex2d.boundTexUnit_)),
		  pData_(tex2d.pData_),
		  width_(std::move(tex2d.width_)),
		  height_(std::move(tex2d.height_)),
		  numChannels_(std::move(tex2d.numChannels_)),
		  isBound_(std::move(tex2d.isBound_)) {

		Logger::trace("Texture2D(Texture2D&&)");

		tex2d.pData_ = nullptr;
	}

	// Move operator
	Texture2D& Texture2D::operator=(Texture2D&& tex2d) noexcept {
		Logger::trace("Texture2D::operator=(Texture2D&&)");

		if (this != &tex2d) {
			id_ = std::move(tex2d.id_);
			boundTexUnit_ = std::move(tex2d.boundTexUnit_);
			pData_ = tex2d.pData_,
			width_ = std::move(tex2d.width_);
			height_ = std::move(tex2d.height_);
			numChannels_ = std::move(tex2d.numChannels_);
			isBound_ = std::move(tex2d.isBound_);
		}

		tex2d.pData_ = nullptr;

		return *this;
	}

	Texture2D::~Texture2D() {
		Logger::trace("~Texture2D()");
		glDeleteTextures(1, &id_);
	}

}