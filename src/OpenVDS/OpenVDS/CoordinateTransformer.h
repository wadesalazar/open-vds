#ifndef COORDINATETRANSFORMER_H
#define COORDINATETRANSFORMER_H

#include <OpenVDS/Vector.h>
#include <OpenVDS/VolumeData.h>

#include <cstring>
#include <cmath>

namespace OpenVDS
{

struct M4
{
  DoubleVector4 data[4];
};

inline void fastInvert(M4 &m)
{
    DoubleVector3 cT(m.data[3].X, m.data[3].Y, m.data[3].Z);

    double fA0 = m.data[1].Y*m.data[2].Z-m.data[2].Y*m.data[1].Z;
    double fA1 = m.data[0].Y*m.data[2].Z-m.data[2].Y*m.data[0].Z;
    double fA2 = m.data[0].Y*m.data[1].Z-m.data[1].Y*m.data[0].Z;
    double fA3 = m.data[1].X*m.data[2].Z-m.data[2].X*m.data[1].Z;
    double fA4 = m.data[0].X*m.data[2].Z-m.data[2].X*m.data[0].Z;
    double fA5 = m.data[0].X*m.data[1].Z-m.data[1].X*m.data[0].Z;
    double fA6 = m.data[1].X*m.data[2].Y-m.data[2].X*m.data[1].Y;
    double fA7 = m.data[0].X*m.data[2].Y-m.data[2].X*m.data[0].Y;
    double fA8 = m.data[0].X*m.data[1].Y-m.data[1].X*m.data[0].Y;

    double fDet = m.data[0].X*fA0 - m.data[0].Y*fA3 + m.data[0].Z*fA6;

    const double  rEpsilon = 1e-60;

    if (fabs(fDet) <= rEpsilon)
    {
      m = {};
      return;
    }

    double  fInvDet = ((double)1.0) / fDet;

    m.data[0].X = fA0*fInvDet;
    m.data[0].Y = -fA1*fInvDet;
    m.data[0].Z = fA2*fInvDet;

    m.data[1].X = -fA3*fInvDet;
    m.data[1].Y = fA4*fInvDet;
    m.data[1].Z = -fA5*fInvDet;

    m.data[2].X = fA6*fInvDet;
    m.data[2].Y = -fA7*fInvDet;
    m.data[2].Z = fA8*fInvDet;

    m.data[3].X = -cT.X*m.data[0].X-cT.Y*m.data[1].X-cT.Z*m.data[2].X;
    m.data[3].Y = -cT.X*m.data[0].Y-cT.Y*m.data[1].Y-cT.Z*m.data[2].Y;
    m.data[3].Z = -cT.X*m.data[0].Z-cT.Y*m.data[1].Z-cT.Z*m.data[2].Z;
}

struct IJKGridDefinition
{
  IJKGridDefinition()
    : origin(0,0,0)
    , iUnitStep(1,0,0)
    , jUnitStep(0,1,0)
    , kUnitStep(0,0,1)
  {}

  IJKGridDefinition(DoubleVector3 origin, DoubleVector3 iUnitStep, DoubleVector3 jUnitStep, DoubleVector3 kUnitStep)
    : origin(origin)
    , iUnitStep(iUnitStep)
    , jUnitStep(jUnitStep)
    , kUnitStep(kUnitStep)
  {}

  DoubleVector3 origin;
  DoubleVector3 iUnitStep;
  DoubleVector3 jUnitStep;
  DoubleVector3 kUnitStep;
};

struct VDSIJKGridDefinition : public IJKGridDefinition
{
  VDSIJKGridDefinition()
    : IJKGridDefinition()
    , dimensionMap(2,1,0)
  {}
  VDSIJKGridDefinition(const IJKGridDefinition &ijkGridDefinition, IntVector3 dimensionMap)
    : IJKGridDefinition(ijkGridDefinition)
    , dimensionMap(dimensionMap)
  {}
  IntVector3 dimensionMap;
};

template<int N>
struct VDSCoordinateTransformerBase
{
  float ijkToWorldTransform[3][3]; ///< Coordinate transfrom matrix
  float worldToIJKTransform[3][3]; ///< Coordinate transform matrix
  float ijkToWorldTranslation[3];  ///< Coordinate translation vector
  float worldToIJKTranslation[3];  ///< Coordinate translation vector

  int  ijkDimensionMap[3]; ///< Map to determine which dimension map to I, J, and K

  ///
  /// Converts a voxel index to world coordinates using the indexer's IJK grid definition and IJK dimension map
  /// @param iVoxelIndex the voxel index to convert
  /// @return the world coordinates
  ///
  FloatVector<3> VoxelIndexToWorldCoordinates(const IntVector<N> &iVoxelIndex) const
  {
    //remap volume index to ijk
    int ijk[3];
    ijk[0] = iVoxelIndex[ijkDimensionMap[0]];
    ijk[1] = iVoxelIndex[ijkDimensionMap[1]];
    if (ijkDimensionMap[2] >= 0)
      ijk[2] = iVoxelIndex[ijkDimensionMap[2]];
    else
      ijk[2] = 0;

    FloatVector<3> rWorldCoords;
    rWorldCoords[0] = ijkToWorldTranslation[0] +
      ijk[0] * ijkToWorldTransform[0][0] +
      ijk[1] * ijkToWorldTransform[1][0] +
      ijk[2] * ijkToWorldTransform[2][0];
    rWorldCoords[1] = ijkToWorldTranslation[1] +
      ijk[0] * ijkToWorldTransform[0][1] +
      ijk[1] * ijkToWorldTransform[1][1] +
      ijk[2] * ijkToWorldTransform[2][1];
    rWorldCoords[2] = ijkToWorldTranslation[2] +
      ijk[0] * ijkToWorldTransform[0][2] +
      ijk[1] * ijkToWorldTransform[1][2] +
      ijk[2] * ijkToWorldTransform[2][2];
    return rWorldCoords;
  }

  ///
  /// Converts a float voxel index to world coordinates using the indexer's IJK grid definition and IJK dimension map
  /// @param rVoxelIndex the float voxel index to convert
  /// @return the world coordinates
  ///
  FloatVector<3> VoxelIndexToWorldCoordinates(const FloatVector<N> &rVoxelIndex) const
  {
    //remap volume index to ijk
    float ijk[3];
    ijk[0] = rVoxelIndex[ijkDimensionMap[0]];
    ijk[1] = rVoxelIndex[ijkDimensionMap[1]];
    if (ijkDimensionMap[2] >= 0)
      ijk[2] = rVoxelIndex[ijkDimensionMap[2]];
    else
      ijk[2] = 0;

    FloatVector<3> rWorldCoords;
    rWorldCoords[0] = ijkToWorldTranslation[0] +
      ijk[0] * ijkToWorldTransform[0][0] +
      ijk[1] * ijkToWorldTransform[1][0] +
      ijk[2] * ijkToWorldTransform[2][0];
    rWorldCoords[1] = ijkToWorldTranslation[1] +
      ijk[0] * ijkToWorldTransform[0][1] +
      ijk[1] * ijkToWorldTransform[1][1] +
      ijk[2] * ijkToWorldTransform[2][1];
    rWorldCoords[2] = ijkToWorldTranslation[2] +
      ijk[0] * ijkToWorldTransform[0][2] +
      ijk[1] * ijkToWorldTransform[1][2] +
      ijk[2] * ijkToWorldTransform[2][2];
    return rWorldCoords;
  }

  ///
  /// Converts world coordinates to a voxel index, rounding to the nearest integer
  /// @param rWorldCoords the world coordinates to convert
  /// @param anVoxelMin
  /// @return the voxel index
  ///
  IntVector<N> WorldCoordinatesToVoxelIndex(const FloatVector<3> &rWorldCoords, const int (&anVoxelMin)[Dimensionality_Max]) const
  {
    int ijk[3];
    ijk[0] = (int)((worldToIJKTranslation[0] +
      rWorldCoords[0] * worldToIJKTransform[0][0] +
      rWorldCoords[1] * worldToIJKTransform[1][0] +
      rWorldCoords[2] * worldToIJKTransform[2][0]) + 0.5f);
    ijk[1] = (int)((worldToIJKTranslation[1] +
      rWorldCoords[0] * worldToIJKTransform[0][1] +
      rWorldCoords[1] * worldToIJKTransform[1][1] +
      rWorldCoords[2] * worldToIJKTransform[2][1]) + 0.5f);
    ijk[2] = (int)((worldToIJKTranslation[2] +
      rWorldCoords[0] * worldToIJKTransform[0][2] +
      rWorldCoords[1] * worldToIJKTransform[1][2] +
      rWorldCoords[2] * worldToIJKTransform[2][2]) + 0.5f);

    //remap ijk to volume index
    IntVector<N> iVoxelIndex;
    for (int i = 0; i < N; i++)
      iVoxelIndex[i] = anVoxelMin[i];
    
    iVoxelIndex[ijkDimensionMap[0]] = ijk[0];
    iVoxelIndex[ijkDimensionMap[1]] = ijk[1];
    if (ijkDimensionMap[2] >= 0)
      iVoxelIndex[ijkDimensionMap[2]] = ijk[2];

    return iVoxelIndex;
  }

  ///
  /// Converts world coordinates to a float voxel index without rounding
  /// @param rWorldCoords the world coordinates to convert
  /// @param anVoxelMin
  /// @return the float voxel index
  ///
  FloatVector<N> WorldCoordinatesToVoxelIndexFloat(const FloatVector<3> &rWorldCoords, const int (&anVoxelMin)[Dimensionality_Max]) const
  {
    float ijk[3];
    ijk[0] = worldToIJKTranslation[0] +
      rWorldCoords[0] * worldToIJKTransform[0][0] +
      rWorldCoords[1] * worldToIJKTransform[1][0] +
      rWorldCoords[2] * worldToIJKTransform[2][0];
    ijk[1] = worldToIJKTranslation[1] +
      rWorldCoords[0] * worldToIJKTransform[0][1] +
      rWorldCoords[1] * worldToIJKTransform[1][1] +
      rWorldCoords[2] * worldToIJKTransform[2][1];
    ijk[2] = worldToIJKTranslation[2] +
      rWorldCoords[0] * worldToIJKTransform[0][2] +
      rWorldCoords[1] * worldToIJKTransform[1][2] +
      rWorldCoords[2] * worldToIJKTransform[2][2];

    //remap ijk to volume index
    FloatVector<N> rVoxelIndex;
    for (int i = 0; i < N; i++)
      rVoxelIndex[i] = (float)anVoxelMin[i];

    rVoxelIndex[ijkDimensionMap[0]] = ijk[0];
    rVoxelIndex[ijkDimensionMap[1]] = ijk[1];
    if (ijkDimensionMap[2] >= 0)
      rVoxelIndex[ijkDimensionMap[2]] = ijk[2];

    return rVoxelIndex;
  }

///////////////////////////// Constructors /////////////////////////////
  
  ///
  /// Constructor
  /// @param cIJKGridAndDImensionMap the IJK grid definition and IJK dimension map
  ///
  VDSCoordinateTransformerBase(const VDSIJKGridDefinition& vdsIjkGridDefinition)
  {
    SetGridDefinition(vdsIjkGridDefinition);
  }

  ///
  /// Constructor
  ///
  VDSCoordinateTransformerBase()
  {
    VDSIJKGridDefinition unitDefinition(IJKGridDefinition(DoubleVector3(0.0, 0.0, 0.0),
                                                          DoubleVector3(1.0, 0.0, 0.0),
                                                          DoubleVector3(0.0, 1.0, 0.0),
                                                          DoubleVector3(0.0, 0.0, 1.0)),
                                        IntVector3(0, 1, 2));
    SetGridDefinition(unitDefinition);
  }

private:  

  ///
  /// Sets the IJK grid definition and IJK dimension map for use in world coordinate conversions
  /// @see VoxelIndexToWorldCoordinates
  /// @see WorldCoordinatesToVoxelIndex
  /// @see WorldCoordinatesToVoxelIndexFloat
  /// @see LocalIndexToWorldCoordinates
  /// @see WorldCoordinatesToLocalIndex
  /// @param vdsIjkGridDefinition the IJK grid definition and IJK dimension map
  ///
  void SetGridDefinition(const VDSIJKGridDefinition& vdsIjkGridDefinition)
  {
    ijkDimensionMap[0] = vdsIjkGridDefinition.dimensionMap.X;
    ijkDimensionMap[1] = vdsIjkGridDefinition.dimensionMap.Y;
    ijkDimensionMap[2] = vdsIjkGridDefinition.dimensionMap.Z;

    ijkToWorldTransform[0][0] = (float)vdsIjkGridDefinition.iUnitStep.X;
    ijkToWorldTransform[0][1] = (float)vdsIjkGridDefinition.iUnitStep.Y;
    ijkToWorldTransform[0][2] = (float)vdsIjkGridDefinition.iUnitStep.Z;

    ijkToWorldTransform[1][0] = (float)vdsIjkGridDefinition.jUnitStep.X;
    ijkToWorldTransform[1][1] = (float)vdsIjkGridDefinition.jUnitStep.Y;
    ijkToWorldTransform[1][2] = (float)vdsIjkGridDefinition.jUnitStep.Z;

    ijkToWorldTransform[2][0] = (float)vdsIjkGridDefinition.kUnitStep.X;
    ijkToWorldTransform[2][1] = (float)vdsIjkGridDefinition.kUnitStep.Y;
    ijkToWorldTransform[2][2] = (float)vdsIjkGridDefinition.kUnitStep.Z;

    ijkToWorldTranslation[0] = (float)vdsIjkGridDefinition.origin.X;
    ijkToWorldTranslation[1] = (float)vdsIjkGridDefinition.origin.Y;
    ijkToWorldTranslation[2] = (float)vdsIjkGridDefinition.origin.Z;

    M4 matrix = {
      {vdsIjkGridDefinition.iUnitStep.X, vdsIjkGridDefinition.iUnitStep.Y, vdsIjkGridDefinition.iUnitStep.Z, 0},
      {vdsIjkGridDefinition.jUnitStep.X, vdsIjkGridDefinition.jUnitStep.Y, vdsIjkGridDefinition.jUnitStep.Z, 0},
      {vdsIjkGridDefinition.kUnitStep.X, vdsIjkGridDefinition.kUnitStep.Y, vdsIjkGridDefinition.kUnitStep.Z, 0},
      {vdsIjkGridDefinition.origin.X,    vdsIjkGridDefinition.origin.Y,    vdsIjkGridDefinition.origin.Z,    1}
    };

    fastInvert(matrix);

    worldToIJKTransform[0][0] = (float)matrix.data[0].X;
    worldToIJKTransform[0][1] = (float)matrix.data[0].Y;
    worldToIJKTransform[0][2] = (float)matrix.data[0].Z;
    worldToIJKTransform[1][0] = (float)matrix.data[1].X;
    worldToIJKTransform[1][1] = (float)matrix.data[1].Y;
    worldToIJKTransform[1][2] = (float)matrix.data[1].Z;
    worldToIJKTransform[2][0] = (float)matrix.data[2].X;
    worldToIJKTransform[2][1] = (float)matrix.data[2].Y;
    worldToIJKTransform[2][2] = (float)matrix.data[2].Z;

    worldToIJKTranslation[0] = (float)matrix.data[3].X;
    worldToIJKTranslation[1] = (float)matrix.data[3].Y;
    worldToIJKTranslation[2] = (float)matrix.data[3].Z;
  }

};

typedef VDSCoordinateTransformerBase<2> VDSCoordinateTransformer2D;
typedef VDSCoordinateTransformerBase<3> VDSCoordinateTransformer3D;
typedef VDSCoordinateTransformerBase<4> VDSCoordinateTransformer4D;
typedef VDSCoordinateTransformerBase<5> VDSCoordinateTransformer5D;
typedef VDSCoordinateTransformerBase<6> VDSCoordinateTransformer6D;

}

#endif //COORDINATETRANSFORMER_H
