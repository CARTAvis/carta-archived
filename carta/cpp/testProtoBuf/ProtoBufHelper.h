#ifndef ProtoBuffHelper_h
#define ProtoBuffHelper_h

#include <google/protobuf/util/time_util.h>
#include "raster_image.pb.h"

typedef ::google::protobuf::uint32 uint32;

void PromptForRasterImageData(Stream::RasterImageData* RasterImageData, uint32 file_id, uint32 layer_id, uint32 x, uint32 y,
                              uint32 width, uint32 height, uint32 stokes, uint32 channel, uint32 mip,
                              Stream::RasterImageData::CompressionType type, float compression_quality, uint32 num_subsets,
                              const std::vector<float> data);

void ListData(const Stream::DataBook& DataBook);

#endif
