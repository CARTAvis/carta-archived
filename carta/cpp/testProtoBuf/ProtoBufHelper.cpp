#include <ctime>
#include <google/protobuf/util/time_util.h>
#include <iostream>
#include <string>

#include "core/Data/DataLoader.h"

#include "stream.pb.h"
#include "request.pb.h"

#include <QString>
#include <sstream>

const std::string NAN_VALUE = "-9999";

typedef ::google::protobuf::uint32 uint32;

// This function fills in a RasterImageData message based on user input.
void PromptForRasterImageData(Stream::RasterImageData* RasterImageData, uint32 file_id, uint32 layer_id, uint32 x, uint32 y,
                              uint32 width, uint32 height, uint32 stokes, uint32 channel, uint32 mip,
                              Stream::RasterImageData::CompressionType type, float compression_quality, uint32 num_subsets,
                              const std::vector<float> rawdata) {
    // The file ID that the raster image corresponds to
    RasterImageData->set_file_id(file_id);

    // The layer ID that the raster image corresponds to
    RasterImageData->set_layer_id(layer_id);

    // Starting x-coordinate of the image region in image space
    RasterImageData->set_x(x);

    // Starting y-coordinate of the image region in image space
    RasterImageData->set_y(y);

    // Width (in the x-direction) of the image region in image space
    RasterImageData->set_width(width);

    // Height (in the y-direction) of the image region in image space
    RasterImageData->set_height(height);

    // The image stokes coordinate
    RasterImageData->set_stokes(stokes);

    // The image channel (z-coordinate)
    RasterImageData->set_channel(channel);

    // The mip level used. The mip level defines how many image pixels correspond to the downsampled image
    RasterImageData->set_mip(mip);

    // The compression algorithm used.
    //RasterImageData->set_compression_type(Stream::RasterImageData::NONE);
    RasterImageData->set_compression_type(type);

    // Compression quality switch
    RasterImageData->set_compression_quality(compression_quality);

    // The number of subsets that the data is broken into (for multithreaded compression/decompression)
    RasterImageData->set_num_subsets(num_subsets);

    for (int i = 0; i < rawdata.size(); i++) {
        std::ostringstream strs;
        strs << rawdata[i];
        std::string str = strs.str();
        if (str == NAN_VALUE) {
            // Run-length encodings of NaN values removed before compression. These values are used to restore the image’s NaN values after decompression.
            std::string index = std::to_string(i);
            RasterImageData->add_nan_encodings(index);
        } else {
            // Image data. For uncompressed data, this is converted into FP32, while for compressed data, this is passed to the compression library for decompression.
            RasterImageData->add_image_data(str);
        }
    }
}

// Iterates though all raster_image_data in the databook and prints info about them.
void ListRasterImageData(const Stream::DataBook& DataBook) {
    const Stream::RasterImageData& RasterImageData = DataBook.raster_image_data();
    std::cout << "############# Check protocol buffer information #############" << std::endl;
    std::cout << "file_id: " << RasterImageData.file_id() << std::endl;
    std::cout << "layer_id: " << RasterImageData.layer_id() << std::endl;
    std::cout << "x: " << RasterImageData.x() << std::endl;
    std::cout << "y: " << RasterImageData.y() << std::endl;
    std::cout << "width: " << RasterImageData.width() << std::endl;
    std::cout << "height: " << RasterImageData.height() << std::endl;
    std::cout << "stoke: " << RasterImageData.stokes() << std::endl;
    std::cout << "channel: " << RasterImageData.channel() << std::endl;
    std::cout << "mip: " << RasterImageData.mip() << std::endl;

    std::cout << "compression_type: ";
    ::Stream::RasterImageData_CompressionType compression_type = RasterImageData.compression_type();
    switch (compression_type) {
        case (::Stream::RasterImageData_CompressionType::RasterImageData_CompressionType_NONE):
            std::cout << "NONE" << std::endl;
            break;
        case (::Stream::RasterImageData_CompressionType::RasterImageData_CompressionType_ZFP):
            std::cout << "ZFP" << std::endl;
            break;
        case (::Stream::RasterImageData_CompressionType::RasterImageData_CompressionType_SZ):
            std::cout << "SZ" << std::endl;
            break;
    }

    std::cout << "compression_quality: " << RasterImageData.compression_quality() << std::endl;
    std::cout << "num_subsets: " << RasterImageData.num_subsets() << std::endl;
    std::cout << "image_data size: " << RasterImageData.image_data_size() << std::endl;
    std::cout << "nan_encodings size: " << RasterImageData.nan_encodings_size() << std::endl;
    std::cout << "#############################################################" << std::endl;

    /// print out the image_data
    //for (int i = 0; i < RasterImageData.image_data_size(); i++) {
    //  string pixel = RasterImageData.image_data(i);
    //  cout << "image_data[" << i << "] = "<< pixel << endl;
    //}

    /// print out the nan_encodings data
    //for (int i = 0; i < RasterImageData.nan_encodings_size(); i++) {
    //  string nan_index = RasterImageData.nan_encodings(i);
    //  cout << "NaN index: " << nan_index << endl;
    //}

    /// example of return object as a QString for the callback function
    //std::string data;
    //RasterImageData.SerializeToString(&data);
    //QString result;
    //result = QString::fromStdString(data);
}

void PromptForRasterImageData(Request::FileListResponse* FileListResponse, std::string request_id, bool success,
                              std::vector<Carta::Data::DataLoader::FileInfo> fileLists, std::vector<QString> dirLists) {
    // Unique request ID
    FileListResponse->set_request_id(request_id);
    // Defines whether the FILE_LIST_REQUEST was successful
    FileListResponse->set_success(success);

    // List of available image files, with file type information and size information

    // List of available subdirectories


}
