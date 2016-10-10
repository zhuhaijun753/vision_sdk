#ifndef PROC_H
#define PROC_H

#include <iostream>
#include <fstream>
#include <ostream>
#include <string>

using namespace std;

enum ProcType {
    IPU1_0, IPU1_1, IPU2, A15, DSP1, DSP2, EVE1, EVE2, EVE3, EVE4, NUMPROC
};
static string procName[] = { "IPU1_0", "IPU1_1", "IPU2", "A15_0", "DSP1", "DSP2",
        "EVE1", "EVE2", "EVE3", "EVE4" };

enum ClassType {
    cAVBReceive,
    cAlg,
    cAlg_ColorToGray,
    cAlg_DMASwMs,
    cAlg_DenseOptFlow,
    cAlg_EdgeDetect,
    cAlg_SoftIsp,
    cAlg_FrameCopy,
    cAlg_SafeFrameCopy,
    cAlg_MyAlgFinish,
    cAlg_MyAlg1,
    cAlg_MyAlg2,
    cAlg_MyAlg3,
    cAlg_Census,
    cAlg_DisparityHamDist,
    cAlg_UltrasonicFusion,
    cAlg_FeaturePlaneComputation,
    cAlg_ImagePreProcess,
    cAlg_ImgPyramid,
    cAlg_FPCompute,
    cAlg_ObjectDetection,
    cAlg_ObjectClassification,
    cAlg_GeoAlign,
    cAlg_GeoAlign3D,
    cAlg_ObjectDraw,
    cAlg_PhotoAlign,
    cAlg_Synthesis,
    cAlg_SparseOpticalFlow,
    cAlg_SfmLinearTwoView,
    cAlg_SfmMain,
    cAlg_SfmMap,
    cAlg_LaneDetect,
    cAlg_Clr,
    cAlg_Sfm,
    cAlg_Fcw,
    cAlg_VectoImg,
    cAlg_IssAewb,
    cAlg_Crc,
    cAlg_DeWarp,
    cAlg_OpenCVCanny,
    cCapture,
    cIssCapture,
    cIssM2mIsp,
    cIssM2mSimcop,
    cUltrasonicCapture,
    cDecode,
    cDisplayCtrl,
    cDisplay,
    cDup,
    cSplit,
    cGate,
    cEncode,
    cGrpxSrc,
    cIPCIn,
    cIPCOut,
    cMerge,
    cNull,
    cNullSource,
    cSelect,
    cSgxFrmcpy,
    cDrmDisplay,
    cSgx3Dsrv,
    cSgx3Dsfm,
    cSync,
    cVPE,
    cHcf,
    cDefLink,
    cAlg_SubframeCopy,
    cAlg_RemapMerge,
    cAlg_StereoPostProcess,
    cAlg_SceneObstructionDetect,
    cAlg_AutoChartDetect,
    cAlg_PoseEstimate,
    cEpLink,
    ClassCount
};
//Last enum used to keep count

class Processor {
    int objsAsgn[NUMPROC][ClassCount]; //holds no of objs assigned the Proc
    int objCount[NUMPROC]; //Total no of objs assigned to a processor
    int linkIDAsgn[ClassCount]; //TODO: Where to put and see if better idea

public:
    Processor();
    int getProcID(ProcType pType, ClassType cType, string name);
    int getObjsAssgn(ProcType pType);
    string getLinkID(ProcType pType, ClassType cType, int procID, string name);
};

#endif
