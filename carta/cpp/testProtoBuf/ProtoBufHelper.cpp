#include <ctime>
#include <google/protobuf/util/time_util.h>
#include <iostream>
#include <string>

#include "core/Data/DataLoader.h"

#include "stream.pb.h"
#include "request.pb.h"

#include <QString>
#include <sstream>

typedef ::google::protobuf::uint32 uint32;
typedef ::google::protobuf::uint64 uint64;

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

    // Image data. For uncompressed data, this is converted into FP32, while for compressed data, this is passed to the compression library for decompression
    RasterImageData->add_image_data(rawdata.data(), rawdata.size() * sizeof(float));
}

// Iterates though all raster_image_data in the databook and prints info about them.
void ListRasterImageData(const Stream::DataBook& DataBook) {
    const Stream::RasterImageData& RasterImageData = DataBook.raster_image_data();
    std::cout << "\n############# Check protocol buffer for raster image #############" << std::endl;
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
    std::cout << "##################################################################" << std::endl;

    /// print out the image_data
    //for (int i = 0; i < RasterImageData.image_data_size(); i++) {
    //  std::string pixel = RasterImageData.image_data(i);
    //  std::cout << "image_data[" << i << "] = "<< pixel << std::endl;
    //}

    /// print out the nan_encodings data
    //for (int i = 0; i < RasterImageData.nan_encodings_size(); i++) {
    //  std::string nan_index = RasterImageData.nan_encodings(i);
    //  std::cout << "NaN index: " << nan_index << std::endl;
    //}

    /// example of return object as a QString for the callback function
    //std::string data;
    //RasterImageData.SerializeToString(&data);
    //QString result;
    //result = QString::fromStdString(data);
}

void PromptForFileListRequestData(Request::FileListResponse* FileListResponse, std::string request_id, bool success,
                              std::vector<Carta::Data::DataLoader::FileInfo> fileLists, std::vector<QString> dirLists) {
    // Unique request ID
    FileListResponse->set_request_id(request_id);
    // Defines whether the FILE_LIST_REQUEST was successful
    FileListResponse->set_success(success);

    // List of available image files, with file type information and size information
    int index = 0;
    for (auto fileList : fileLists) {
        // set file name
        QString name = fileList.name;
        std::string nameUtf8 = name.toUtf8().constData();
        //FileListResponse->add_files()->set_name(nameUtf8); // convert QString to std::string
        FileListResponse->add_files();
        FileListResponse->mutable_files(index)->set_name(nameUtf8); // convert QString to std::string
        // set file type
        int type = fileList.type;
        switch (type) {
            case 0:
                FileListResponse->mutable_files(index)->set_type(::Request::FileInfo_FileType::FileInfo_FileType_FITS);
                break;
            case 1:
                FileListResponse->mutable_files(index)->set_type(::Request::FileInfo_FileType::FileInfo_FileType_CASA);
                break;
            case 2:
                FileListResponse->mutable_files(index)->set_type(::Request::FileInfo_FileType::FileInfo_FileType_HDF5);
        }
        // set file size
        uint64 size = fileList.size;
        FileListResponse->mutable_files(index)->set_size(size);
        index++;
    }

    // List of available subdirectories
    for (QString dirList : dirLists) {
        // convert QString to std::string
        std::string dirListUtf8 = dirList.toUtf8().constData();
        FileListResponse->add_directories(dirListUtf8);
    }
}

void ListFileListRequestData(const Request::DataBook& DataBook) {
    const Request::FileListResponse& FileListRequestData = DataBook.file_list_data();

    std::cout << "\n############# Check protocol buffer for file lists ###############" << std::endl;

    /// print out the list of image files that can be opened
    for (int i = 0; i < FileListRequestData.files_size(); i++) {
        // get file name
        std::string fileName = FileListRequestData.files(i).name();
        // get file type
        ::Request::FileInfo_FileType fileType = FileListRequestData.files(i).type();
        std::string fileTypeString;
        switch (fileType) {
            case (::Request::FileInfo_FileType::FileInfo_FileType_FITS):
                fileTypeString = "FITS";
                break;
            case (::Request::FileInfo_FileType::FileInfo_FileType_CASA):
                fileTypeString = "CASA";
                break;
            case (::Request::FileInfo_FileType::FileInfo_FileType_HDF5):
                fileTypeString = "HDF5";
        }
        // get file size
        uint64 fileSize = FileListRequestData.files(i).size();
        std::cout << "fileName[" << i << "] = "<< fileName
                  << ", fileType[" << i << "] = " << fileTypeString
                  << ", fileSize[" << i << "] = " << fileSize << std::endl;
    }

    /// print out the list of sub dirs
    for (int i = 0; i < FileListRequestData.directories_size(); i++) {
        std::string subDir = FileListRequestData.directories(i);
        std::cout << "subDir[" << i << "] = "<< subDir << std::endl;
    }

    std::cout << "##################################################################" << std::endl;
}
