#ifndef CartaHelper_h
#define CartaHelper_h

int getAxisIndex(std::shared_ptr<Carta::Lib::Image::ImageInterface> m_image,
                 Carta::Lib::AxisInfo::KnownType axisType);

Carta::Lib::NdArray::RawViewInterface* getRawData(std::shared_ptr<Carta::Lib::Image::ImageInterface> m_image,
                                                  int frameStart, int frameEnd, int stokeSliceIndex);

std::vector<double> getHertzValues(std::shared_ptr<Carta::Lib::Image::ImageInterface> m_image,
                                   std::vector<int> dims, int spectralIndex);

std::vector<double> extractRawData(std::shared_ptr<Carta::Lib::Image::ImageInterface> astroImage,
                                   int frameStart, int frameEnd, int stokeIndex, int mip);

bool downVector(std::vector<double> &rawData, int x_size, int y_size, int mip);

std::vector<double> downSampling(Carta::Lib::NdArray::RawViewInterface *view, int ilb, int iub, int jlb, int jub, int mip);

void compareVectors(std::vector<double> vec1, std::vector<double> vec2);

#endif
