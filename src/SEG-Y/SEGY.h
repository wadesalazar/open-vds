/****************************************************************************
** Copyright 2019 The Open Group
** Copyright 2019 Bluware, Inc.
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
****************************************************************************/

#include <cstddef>

namespace SEGY
{

enum
{
  TextualFileHeaderColumns = 80,
  TextualFileHeaderLines = 40,
  TextualFileHeaderSize = TextualFileHeaderColumns * TextualFileHeaderLines,
  BinaryFileHeaderSize = 400,
  TraceHeaderSize = 240
};

enum class FieldWidth
{
  TwoByte,
  FourByte
};

enum class Endianness
{
  BigEndian,
  LittleEndian
};

struct HeaderField
{
  int        byteLocation;
  enum FieldWidth fieldWidth;

  HeaderField() = default;
  HeaderField(int byteLocation, FieldWidth fieldWidth) : byteLocation(byteLocation), fieldWidth(fieldWidth) {}

  bool       Defined() const { return byteLocation != 0; }
};

namespace BinaryHeader
{
enum class DataSampleFormatCode
{
  Unknown    =  0,
  IBMFloat   =  1, // 4-byte IBM floating-point
  Int32      =  2, // 4-byte, two's complement integer
  Int16      =  3, // 2-byte, two's complement integer
  FixedPoint =  4, // 4-byte fixed-point with gain (obsolete)
  IEEEFloat  =  5, // 4-byte IEEE floating-point
  IEEEDouble =  6, // 8-byte IEEE floating-point
  Int24      =  7, // 3-byte, two's complement integer
  Int8       =  8, // 1-byte, two's complement integer
  Int64      =  9, // 8-byte, two's complement integer
  UInt32     = 10, // 4-byte, unsigned integer
  UInt16     = 11, // 2-byte, unsigned integer
  UInt64     = 12, // 8-byte, unsigned integer
  UInt24     = 15, // 3-byte, unsigned integer
  UInt8      = 16  // 1-byte, unsigned integer
};

enum class SortCode
{
  Other                          = -1, // Other (should be explained in a user Extended Textual File Header stanza)
  Unknown                        =  0, // Unknown
  As_Recorded                    =  1, // As recorded (no sorting)
  CDP_Ensemble                   =  2, // CDP ensemble
  Single_Fold_Continuous_Profile =  3, // Single fold continuous profile
  Horizontally_Stacked           =  4, // Horizontally stacked
  Common_Source_Point            =  5, // Common source point
  Common_Receiver_Point          =  6, // Common receiver point
  Common_Offset_Point            =  7, // Common offset point
  Common_Mid_Point               =  8, // Common mid-point
  Common_Conversion_Point        =  9  // Common conversion point
};

enum class MeasurementSystem
{
  Unknown  = 0,
  Meters   = 1,
  Feet     = 2
};

// Standard header fields
static const HeaderField TracesPerEnsembleHeaderField(13, FieldWidth::TwoByte);
static const HeaderField AuxiliaryTracesPerEnsembleHeaderField(15, FieldWidth::TwoByte);
static const HeaderField SampleIntervalHeaderField(17, FieldWidth::TwoByte);
static const HeaderField NumSamplesHeaderField(21, FieldWidth::TwoByte);
static const HeaderField DataSampleFormatCodeHeaderField(25, FieldWidth::TwoByte);
static const HeaderField EnsembleFoldHeaderField(27, FieldWidth::TwoByte);
static const HeaderField TraceSortingCodeHeaderField(29, FieldWidth::TwoByte);
static const HeaderField MeasurementSystemHeaderField(55, FieldWidth::TwoByte);
static const HeaderField SEGYFormatRevisionNumberHeaderField(301, FieldWidth::TwoByte);
static const HeaderField FixedLengthTraceFlagHeaderField(303, FieldWidth::TwoByte);
static const HeaderField ExtendedTextualFileHeaderCountHeaderField(305, FieldWidth::TwoByte);

} // end namespace BinaryHeader

namespace TraceHeader
{
enum class TraceIdentificationCode
{
Other                                                  = -1, // Other
Unknown                                                =  0, // Unknown
TimeDomainSeismicData                                  =  1, // Time domain seismic data
Dead                                                   =  2, // Dead
Dummy                                                  =  3, // Dummy
TimeBreak                                              =  4, // Time break
Uphole                                                 =  5, // Uphole
Sweep                                                  =  6, // Sweep
Timing                                                 =  7, // Timing
Waterbreak                                             =  8, // Waterbreak
NearFieldGunSignature                                  =  9, // Near-field gun signature
FarFieldGunSignature                                   = 10, // Far-field gun signature
SeismicPressureSensor                                  = 11, // Seismic pressure sensor
MulticomponentSeismicSensor_VerticalComponent          = 12, // Multicomponent seismic sensor - Vertical component
MulticomponentSeismicSensor_CrosslineComponent         = 13, // Multicomponent seismic sensor - Cross-line component
MulticomponentSeismicSensor_InlineComponent            = 14, // Multicomponent seismic sensor - In-line component
RotatedMulticomponentSeismicSensor_VerticalComponent   = 15, // Rotated multicomponent seismic sensor - Vertical component
RotatedMulticomponentSeismicSensor_TransverseComponent = 16, // Rotated multicomponent seismic sensor - Transverse component
RotatedMulticomponentSeismicSensor_RadialComponent     = 17, // Rotated multicomponent seismic sensor - Radial component
VibratorReactionMass                                   = 18, // Vibrator reaction mass
VibratorBaseplate                                      = 19, // Vibrator baseplate
VibratorEstimatedGroundForce                           = 20, // Vibrator estimated ground force
VibratorReference                                      = 21, // Vibrator reference
TimeVelocityPairs                                      = 22, // Time-velocity pairs
TimeDepthPairs                                         = 23, // Time-depth pairs
DepthVelocityPairs                                     = 24, // Depth-velocity pairs
DepthDomainSeismicData                                 = 25, // Depth domain seismic data
GravityPotential                                       = 26, // Gravity potential
ElectricField_VerticalComponent                        = 27, // Electric field - Vertical component
ElectricField_CrosslineComponent                       = 28, // Electric field - Cross-line component
ElectricField_InlineComponent                          = 29, // Electric field - In-line component
RotatedElectricField_VerticalComponent                 = 30, // Rotated electric field - Vertical component
RotatedElectricField_TransverseComponent               = 31, // Rotated electric field - Transverse component
RotatedElectricField_RadialComponent                   = 32, // Rotated electric field - Radial component
MagneticField_VerticalComponent                        = 33, // Magnetic field - Vertical component
MagneticField_CrosslineComponent                       = 34, // Magnetic field - Cross-line component
MagneticField_InlineComponent                          = 35, // Magnetic field - In-line component
RotatedMagneticField_VerticalComponent                 = 36, // Rotated magnetic field - Vertical component
RotatedMagneticField_TransverseComponent               = 37, // Rotated magnetic field - Transverse component
RotatedMagneticField_RadialComponent                   = 38, // Rotated magnetic field - Radial component
RotationalSensor_Pitch                                 = 39, // Rotational sensor - Pitch
RotationalSensor_Roll                                  = 40, // Rotational sensor - Roll
RotationalSensor_Yaw                                   = 41  // Rotational sensor - Yaw
};

enum class CoordinateUnits
{
  Unknown               = 0,
  Length                = 1, // Length (meters or feet as specified in Binary File Header bytes 3255-3256 and in Extended Textual Header if Location Data are included in the file)
  ArcSeconds            = 2, // Seconds of arc (deprecated)
  DecimalDegrees        = 3, // Decimal degrees (preferred degree representation)
  DegreesMinutesSeconds = 4  // Degrees, minutes, seconds (DMS)
};

static const HeaderField TraceSequenceNumberHeaderField(1, FieldWidth::FourByte);
static const HeaderField TraceSequenceNumberWithinFileHeaderField(5, FieldWidth::FourByte);
static const HeaderField EnergySourcePointNumberHeaderField(17, FieldWidth::FourByte);
static const HeaderField EnsembleNumberHeaderField(21, FieldWidth::FourByte);
static const HeaderField TraceNumberWithinEnsembleHeaderField(25, FieldWidth::FourByte);
static const HeaderField TraceIdentificationCodeHeaderField(29, FieldWidth::TwoByte);
static const HeaderField CoordinateScaleHeaderField(71, FieldWidth::TwoByte);
static const HeaderField SourceXCoordinateHeaderField(73, FieldWidth::FourByte);
static const HeaderField SourceYCoordinateHeaderField(77, FieldWidth::FourByte);
static const HeaderField GroupXCoordinateHeaderField(81, FieldWidth::FourByte);
static const HeaderField GroupYCoordinateHeaderField(84, FieldWidth::FourByte);
static const HeaderField CoordinateUnitsHeaderField(89, FieldWidth::TwoByte);
static const HeaderField StartTimeHeaderField(109, FieldWidth::TwoByte);
static const HeaderField NumSamplesHeaderField(115, FieldWidth::TwoByte);
static const HeaderField SampleIntervalHeaderField(117, FieldWidth::TwoByte);
static const HeaderField EnsembleXCoordinateHeaderField(181, FieldWidth::FourByte);
static const HeaderField EnsembleYCoordinateHeaderField(185, FieldWidth::FourByte);
static const HeaderField InlineNumberHeaderField(189, FieldWidth::FourByte);
static const HeaderField CrosslineNumberHeaderField(193, FieldWidth::FourByte);

} // end namespace TraceHeader

// Conversion functions

void Ibm2ieee(void *to, const void *from, size_t len);
void Ieee2ibm(void *to, const void *from, size_t len);

// Read field from header

int ReadFieldFromHeader(const void *header, HeaderField const &headerField, Endianness endianness);

} // end namespace SEGY
