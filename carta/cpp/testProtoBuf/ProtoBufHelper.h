#ifndef ProtoBuffHelper_h
#define ProtoBuffHelper_h

#include <google/protobuf/util/time_util.h>
#include "core/Data/DataLoader.h"

#include "stream.pb.h"

typedef ::google::protobuf::uint32 uint32;

void concentrate(std::string pixel, std::vector<char> &binaryPayloadCache);

void PromptForRasterImageData(Stream::RasterImageData* RasterImageData, uint32 file_id, uint32 layer_id, uint32 x, uint32 y,
                              uint32 width, uint32 height, uint32 stokes, uint32 channel, uint32 mip,
                              Stream::RasterImageData::CompressionType type, float compression_quality, uint32 num_subsets,
                              const std::vector<float> data);

void ListRasterImageData(const Stream::DataBook& DataBook);

void PromptForFileListRequestData(Request::FileListResponse* FileListResponse, std::string request_id, bool success,
                              std::vector<Carta::Data::DataLoader::FileInfo> fileLists, std::vector<QString> dirLists);

#endif
