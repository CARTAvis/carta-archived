#include <ctime>
#include <google/protobuf/util/time_util.h>
#include <iostream>
#include <string>

#include "databook.pb.h"

using namespace std;

// This function fills in a RasterImage message based on user input.
void PromptForRasterImageData(CartaMessage::RasterImage* RasterImage, const std::vector<double> rawdata) {
    int file_id = 0;
    RasterImage->set_file_id(file_id);

    int layer_id = 0;
    RasterImage->set_layer_id(layer_id);

    int image_bound_x = 0;
    RasterImage->set_image_bound_x(image_bound_x);

    int image_bound_y = 0;
    RasterImage->set_image_bound_y(image_bound_y);

    int stoke = 0;
    RasterImage->set_stoke(stoke);

    int channel = 0;
    RasterImage->set_channel(channel);

    for (int i = 0; i < rawdata.size(); i++) {
        RasterImage->add_pixel_data(rawdata[i]);
    }
}

// Iterates though all raster_image in the databook and prints info about them.
void ListData(const CartaMessage::DataBook& DataBook) {
    // list raster image data
    for (int i = 0; i < DataBook.raster_image_size(); i++) {
        const CartaMessage::RasterImage& RasterImage = DataBook.raster_image(i);

        cout << "file_id: " << RasterImage.file_id() << endl;
        cout << "layer_id: " << RasterImage.layer_id() << endl;
        cout << "image_bound_x: " << RasterImage.image_bound_x() << endl;
        cout << "image_bound_y: " << RasterImage.image_bound_y() << endl;
        cout << "stoke: " << RasterImage.stoke() << endl;
        cout << "channel: " << RasterImage.channel() << endl;
        cout << "pixel_data size: " << RasterImage.pixel_data_size() << endl;

        //for (int i = 0; i < RasterImage.pixel_data_size(); i++) {
        //  double tmp = RasterImage.pixel_data(i);
        //  cout << "pixel_data[" << i << "] = "<< tmp << endl;
        //}
    }
}

