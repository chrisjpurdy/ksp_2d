#pragma once

#include <map>
#include <string>
#include <memory>

#include "SimpleImage.h"

/*
The ImageManager manages loaded images. The main aim is that it avoid re-loading images which are already loaded, sharing them between objects.
Images are loaded as RawImageData objects.
SimpleImage and ImageRenderer are front ends to the image data, specifying which pixels to draw from that image.
Uses smart pointers and maps to maintain the list, so may be worth looking at for that reason?
*/

#include "../sdl2_framework/ImageManager.h"
#include "../rapidjson/filereadstream.h"
#include "../rapidjson/document.h"
#include <cstdio>

struct SpriteData {
    std::string name;
    std::string url;
    int x{};
    int y{};
    int width{};
    int height{};
};

class ImageManager
{
	// Store the data for the images, by URL loaded from
	std::map<std::string, std::weak_ptr<RawImageData> > images;
	// These images are kept permanently loaded
	std::map<std::string, std::shared_ptr<RawImageData> > permanentlyLoadedImages;

	/* KSP IMAGE MANAGER STRUCTURES */
    // TODO need to sort out where the sprite data goes when this gets destroyed
    std::map<std::string, std::weak_ptr<SpriteData> > spriteLookups;

public:
	// Singleton implementation, if desired, use static get() to get the singleton object.
	static ImageManager* get()
	{
		if (g_pManager == nullptr)
            g_pManager = new ImageManager();
		return g_pManager;
	}

	// Load an image into memory, and return a SimpleImage object which uses it
	// imageURL : the URL of the file to load
	// loadPermanently : keep the image in memory even when you stop using it in this image
	static SimpleImage loadImage(std::string imageURL, bool loadPermanently = false)
	{
		return ImageManager::get()->getImagebyURL(imageURL, true/*Load*/, loadPermanently/*Keep it loaded?*/);
	}

	/*
	Destroy the image manager object - note that this will destroy its maps which it contains
	Which will destroy the objects contained.
	*/
	static void destroyImageManager()
	{
		if (g_pManager == nullptr)
			return;
		delete g_pManager;
		g_pManager = nullptr;
	}

private:
	/* Singleton ImageManager object pointer. */
	static ImageManager* g_pManager;

public:

    // Constructor - just initialise the variables
    ImageManager()
    {
        resetOptions();
    }

	// Retrieve an image by its URL. If already loaded then it gets the existing one
	std::shared_ptr<RawImageData> getImagePtrbyURL(std::string imageURL, bool loadIfNotLoaded = false, bool loadPermanently = false)
	{
		//std::cout << "\t\t+++ Find image: " << imageURL << std::endl;

		auto iter = images.find(imageURL);
		if (iter != images.end())
		{
			auto weak = iter->second; // Pull out pointer rather than key
			if (weak.expired())
			{
				// Remove the expired image from the list
				images.erase(iter);
				// And then carry on, to load it again
			}
			else
			{
				//std::cout << "\t\t+++ Find image: " << imageURL << " image found" << std::endl;

				auto shared = weak.lock(); // Convert to a shared pointer, which we are returning
				return shared; // Iterator points at the shared pointer
			}
		}

		// Otherwise either load it or return a null pointer
		if (loadIfNotLoaded)
		{
			std::cout << "\t\t+++ Load image: " << imageURL << std::endl;

			auto newSharedPtr = loadAsNew(imageURL); // Load image from file
			if (newSharedPtr != nullptr)
			{
				std::weak_ptr<RawImageData> weak = newSharedPtr; // Shared to weak pointer
				images.insert(std::pair<std::string, std::weak_ptr<RawImageData>>(imageURL, weak));
				if (loadPermanently) // Storing shared pointer keeps it alive while array exists - forever.
					permanentlyLoadedImages.insert(std::pair<std::string, std::shared_ptr<RawImageData> >(imageURL, newSharedPtr));
			}
			else
			{
				std::cout << "ImageManager::getImagePtrbyURL() : Failed to load image " << imageURL << std::endl;
			}
			return newSharedPtr; // Return the loading attempt result, whether successful or not
		}
		return nullptr; // failed
	}


	// Create a new ImageData object to represent the image and allow manipulation and rendering
	SimpleImage getImagebyURL(std::string imageURL, bool loadIfNotLoaded = false, bool loadPermanently = false)
	{
		return SimpleImage(getImagePtrbyURL(imageURL, loadIfNotLoaded, loadPermanently));
	}


	/*
	Specify whether an image should be permanently loaded or not.
	Keeps a shared pointer to the image data, which will keep it alive
	imageURL : the url of the image file
	makePermanent : true to keep it loaded, false to unload it as soon as nothing is actively using it
	*/
	bool setPermanentlyLoaded(std::string imageURL, bool makePermanent = true)
	{
		if (!makePermanent)
		{	// Take item out of permanentlyLoadedImages
			auto iter = permanentlyLoadedImages.find(imageURL);
			if (iter != permanentlyLoadedImages.end())
			{
				permanentlyLoadedImages.erase(iter);
				return true;
			}
			return false; // Can't remove because not in there
		}
		else
		{	// Find item in normal array and add to permanently loaded images
			// If already in permanentlyLoadedImages then nothing to do.
			auto iter = permanentlyLoadedImages.find(imageURL);
			if (iter != permanentlyLoadedImages.end())
				return true;
			// Otherwise look for the image...
			auto iter2 = images.find(imageURL);
			if (iter2 != images.end())
			{
				permanentlyLoadedImages.insert(std::pair<std::string, std::shared_ptr<RawImageData>>(iter2->first, iter2->second.lock()));
				return true;
			}
			return false;
		}
	}

	// Load the image data into a NEW RawImageData object and return the shared pointer to it
	std::shared_ptr<RawImageData> loadAsNew(std::string imageURL)
	{
		auto* pData = new RawImageData(imageURL);
		if (pData->loadImage(imageURL, 0, cropTop, cropBottom, cropLeft, cropRight))
		{
			return std::shared_ptr<RawImageData>(pData);
		}
		printf( "Failed to load image %s\n", imageURL.c_str() );
		delete pData; // Failed so no longer needed
		return nullptr;
	}

	// Set some cropping for the image loading, e.g. to remove a border from an image
	// Used ONLY when loading images.
	void setImageLoadCropping(int iLeft, int iRight, int iTop, int iBottom)
	{
		cropLeft = iLeft;
		cropRight = iRight;
		cropTop = iTop;
		cropBottom = iBottom;
	}

	// Clear all cropping and rotation options.
	void resetOptions()
	{
		cropLeft = cropRight = cropTop = cropBottom = 0;
	}

	// Create a shrunk copy of another image - DOES NOT CACHE THE IMAGE FOR REUSE!!! See ImageManager for caching version
	SimpleImage shrinkBy(SimpleImage existingImage, int iReductionMultiplier)
	{
		const std::string& strURL = existingImage.getImageURL();
		std::string strNewName = "#S" + std::to_string(iReductionMultiplier) + "#" + strURL;
		// Look for the string already existing now...
		std::shared_ptr<RawImageData> raw = getImagePtrbyURL(strNewName, false/*don't load!*/);
		if (raw != nullptr)
		{
			// Already found this, so just use the one we already have!
			return SimpleImage(raw); // A new ImageData object
		}
		// Otherwise we need to create a new one
		auto* pNewData = new RawImageData(existingImage.getTheData(), iReductionMultiplier);
		// And store it in the array so we find it next time
		std::shared_ptr<RawImageData> newData(pNewData);
		std::weak_ptr<RawImageData> weak(newData);
		images.insert(std::pair<std::string, std::weak_ptr<RawImageData>>(strNewName, weak));
		// The we return a new ImageData which wraps it
		return SimpleImage(newData);
	}

	// Create a re-sized copy of another image
	SimpleImage resizeTo(SimpleImage existingImage, int iNewWidth, int iNewHeight)
	{
		const std::string& strURL = existingImage.getImageURL();
		std::string strNewName = "#R" + std::to_string(iNewWidth) + "#" + std::to_string(iNewHeight) + "#" + strURL;
		// Look for the string already existing now...
		std::shared_ptr<RawImageData> raw = getImagePtrbyURL(strNewName, false/*don't load!*/);
		if (raw != nullptr)
		{
			// Already found this, so just use the one we already have!
			return SimpleImage(raw); // A new ImageData object
		}
		// Otherwise we need to create a new one
		auto* pNewImageData = new RawImageData(iNewWidth, iNewHeight, existingImage.getTheData());
		// And store it in the array so we find it next time
		std::shared_ptr<RawImageData> newPointer(pNewImageData);
		std::weak_ptr<RawImageData> weak(newPointer); // Need to store a weak pointer so it doesn't keep it alive
		images.insert(std::pair<std::string, std::weak_ptr<RawImageData>>(strNewName, weak));
		// The we return a new ImageData which wraps it
		return SimpleImage(newPointer);
	}

	/* KSP IMAGE MANAGER FUNCTIONS */

    std::shared_ptr<SpriteData> getSpriteData(std::string name) {

        auto iter = spriteLookups.find(name);
        if (iter != spriteLookups.end()) {
            auto weak = iter->second;
            if (weak.expired())
            {
                spriteLookups.erase(iter);
            }
            else
            {
                auto shared = weak.lock();
                return shared;
            }
        }

        //std::cout << "\t\t-+- Finding sprite URL: " << name << std::endl;

        FILE* fp = fopen("sprite_loc.json", "r");

        char readBuffer[65536];
        rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

        rapidjson::Document json;
        json.ParseStream(is);

        fclose(fp);

        for (auto& v : json.GetArray()) {
            if (strcmp(v["name"].GetString(), name.c_str()) == 0) {
                //std::cout << "\t\t-+- Found sprite URL: " << name << std::endl;
                auto* sd = new SpriteData();
                sd->name = name;
                sd->url = v["file"].GetString();
                sd->x = v["x"].GetInt();
                sd->y = v["y"].GetInt();
                sd->width = v["width"].GetInt();
                sd->height = v["height"].GetInt();

                std::shared_ptr<SpriteData> sharedPtr(sd);
                std::weak_ptr<SpriteData> weak(sharedPtr);

                spriteLookups.insert(std::pair<std::string, std::weak_ptr<SpriteData>>(name, weak) );
                return sharedPtr;
            }
        }
        std::cout << "KSPImageManager::getPartImageURL() : Couldn't find part name " << name << std::endl;
        return nullptr; // couldn't find the image

    }

    // Make a sprite from an image (presumably a spritesheet)
    SimpleImage getSpriteFromImg(const std::string& imgUrl, std::string name, int x, int y, int width, int height) {

        // Check if already created
        std::shared_ptr<RawImageData> raw = getImagePtrbyURL(name, false);
        if (raw != nullptr) {
            return SimpleImage(raw);
        }

        // load permanently since it is a spritesheet
        auto spritesheet = getImagePtrbyURL(imgUrl, true, true);

        auto* newRaw = new RawImageData(x, y, width, height, spritesheet);
        std::shared_ptr<RawImageData> newPointer(newRaw);
        std::weak_ptr<RawImageData> weak(newPointer);
        images.insert(std::pair<std::string, std::weak_ptr<RawImageData>>(name, weak));
        return SimpleImage(newPointer);
    }

    // Make a sprite from an image (presumably a spritesheet)
    SimpleImage getSpriteFromImg(std::shared_ptr<SpriteData> data) {

        // Check if already created
        std::shared_ptr<RawImageData> raw = getImagePtrbyURL(data->name, false);
        if (raw != nullptr) {
            return SimpleImage(raw);
        }

        // load permanently since it is a spritesheet
        auto spritesheet = getImagePtrbyURL(data->url, true, true);

        auto* newRaw = new RawImageData(data->x, data->y, data->width, data->height, spritesheet);
        std::shared_ptr<RawImageData> newPointer(newRaw);
        std::weak_ptr<RawImageData> weak(newPointer);
        images.insert(std::pair<std::string, std::weak_ptr<RawImageData>>(data->name, weak));
        return SimpleImage(newPointer);
    }


private:
	int cropLeft;
	int cropRight;
	int cropTop;
	int cropBottom;
};

