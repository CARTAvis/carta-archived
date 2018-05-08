#ifndef CartaHelper_h
#define CartaHelper_h

int getAxisIndex(std::shared_ptr<Carta::Lib::Image::ImageInterface> m_image,
                 Carta::Lib::AxisInfo::KnownType axisType);

Carta::Lib::NdArray::RawViewInterface* getRawData(std::shared_ptr<Carta::Lib::Image::ImageInterface> m_image,
                                                  int frameStart, int frameEnd, int stokeSliceIndex);

std::vector<double> getHertzValues(std::shared_ptr<Carta::Lib::Image::ImageInterface> m_image,
                                   std::vector<int> dims, int spectralIndex);

void testMinMax(std::shared_ptr<Carta::Lib::Image::ImageInterface> astroImage);

void testPercentileHistogram(std::shared_ptr<Carta::Lib::Image::ImageInterface> astroImage,
                             Carta::Lib::IPercentilesToPixels<double>::SharedPtr calculator, int numberOfBins);

void testPercentileManku99(std::shared_ptr<Carta::Lib::Image::ImageInterface> astroImage,
                           Carta::Lib::IPercentilesToPixels<double>::SharedPtr calculator,
                           int numBuffers, int bufferCapacity, int sampleAfter);

std::vector<double> extractRawData(std::shared_ptr<Carta::Lib::Image::ImageInterface> astroImage);

#endif
