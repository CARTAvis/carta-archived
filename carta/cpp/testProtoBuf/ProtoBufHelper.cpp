#include <ctime>
#include <google/protobuf/util/time_util.h>
#include <iostream>
#include <string>

#include "raster_image.pb.h"

#include <QString>
#include <sstream>

using namespace std;

typedef ::google::protobuf::uint32 uint32;

// This function fills in a RasterImageData message based on user input.
void PromptForRasterImageData(Stream::RasterImageData* RasterImageData, const std::vector<float> rawdata) {
    // The file ID that the raster image corresponds to
    uint32 file_id = 0;
    RasterImageData->set_file_id(file_id);

    // The layer ID that the raster image corresponds to
    uint32 layer_id = 0;
    RasterImageData->set_layer_id(layer_id);

    // Starting x-coordinate of the image region in image space
    uint32 x = 0;
    RasterImageData->set_x(x);

    // Starting y-coordinate of the image region in image space
    uint32 y = 0;
    RasterImageData->set_y(y);

    // Width (in the x-direction) of the image region in image space
    uint32 width = 1;
    RasterImageData->set_width(width);

    // Height (in the y-direction) of the image region in image space
    uint32 height = 1;
    RasterImageData->set_height(height);

    // The image stokes coordinate
    uint32 stoke = 0;
    RasterImageData->set_stokes(stoke);

    // The image channel (z-coordinate)
    uint32 channel = 0;
    RasterImageData->set_channel(channel);

    // The mip level used. The mip level defines how many image pixels correspond to the downsampled image
    uint32 mip = 1;
    RasterImageData->set_mip(mip);

    // The compression algorithm used.
    RasterImageData->set_compression_type(Stream::RasterImageData::NONE);

    // Compression quality switch
    float compression_quality = 0;
    RasterImageData->set_compression_quality(compression_quality);

    // The number of subsets that the data is broken into (for multithreaded compression/decompression)
    uint32 num_subsets = 0;
    RasterImageData->set_num_subsets(num_subsets);

    for (int i = 0; i < rawdata.size(); i++) {
        std::ostringstream strs;
        strs << rawdata[i];
        std::string str = strs.str();
        if (str == "-1") {
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
void ListData(const Stream::DataBook& DataBook) {
    const Stream::RasterImageData& RasterImageData = DataBook.raster_image_data();
    cout << "############# Check protocol buffer information #############" << endl;
    cout << "file_id: " << RasterImageData.file_id() << endl;
    cout << "layer_id: " << RasterImageData.layer_id() << endl;
    cout << "x: " << RasterImageData.x() << endl;
    cout << "y: " << RasterImageData.y() << endl;
    cout << "width: " << RasterImageData.width() << endl;
    cout << "height: " << RasterImageData.height() << endl;
    cout << "stoke: " << RasterImageData.stokes() << endl;
    cout << "channel: " << RasterImageData.channel() << endl;
    cout << "mip: " << RasterImageData.mip() << endl;

    cout << "compression_type: ";
    ::Stream::RasterImageData_CompressionType compression_type = RasterImageData.compression_type();
    switch (compression_type) {
        case (::Stream::RasterImageData_CompressionType::RasterImageData_CompressionType_NONE):
            cout << "NONE" << endl;
            break;
        case (::Stream::RasterImageData_CompressionType::RasterImageData_CompressionType_ZFP):
            cout << "ZFP" << endl;
            break;
        case (::Stream::RasterImageData_CompressionType::RasterImageData_CompressionType_SZ):
            cout << "SZ" << endl;
            break;
    }

    cout << "compression_quality: " << RasterImageData.compression_quality() << endl;
    cout << "num_subsets: " << RasterImageData.num_subsets() << endl;
    cout << "image_data size: " << RasterImageData.image_data_size() << endl;
    cout << "nan_encodings size: " << RasterImageData.nan_encodings_size() << endl;
    cout << "#############################################################" << endl;

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

