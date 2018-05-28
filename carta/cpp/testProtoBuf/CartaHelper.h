#ifndef CartaHelper_h
#define CartaHelper_h

int getAxisIndex(std::shared_ptr<Carta::Lib::Image::ImageInterface> m_image,
                 Carta::Lib::AxisInfo::KnownType axisType);

Carta::Lib::NdArray::RawViewInterface* getRawData(std::shared_ptr<Carta::Lib::Image::ImageInterface> m_image,
                                                  int frameStart, int frameEnd, int stokeSliceIndex);

std::vector<double> getHertzValues(std::shared_ptr<Carta::Lib::Image::ImageInterface> m_image,
                                   std::vector<int> dims, int spectralIndex);

std::vector<float> extractRawData(std::shared_ptr<Carta::Lib::Image::ImageInterface> astroImage,
                                  int ilb, int iub, int jlb, int jub,
                                  int frameStart, int frameEnd, int stokeIndex, int mip);

bool downVector(std::vector<float> &rawData, int x_size, int y_size, int mip);

std::vector<float> downSampling(Carta::Lib::NdArray::RawViewInterface *view, int ilb, int iub, int jlb, int jub, int mip);

std::vector<float> fullSampling(Carta::Lib::NdArray::RawViewInterface *view);

void compareVectors(std::vector<float> vec1, std::vector<float> vec2);

#endif
