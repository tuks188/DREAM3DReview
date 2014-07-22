#ifndef _TransformationPhaseConstants_H_
#define _TransformationPhaseConstants_H_

#include <QtCore/QString>

namespace TransformationPhase
{
  const QString TransformationPhasePluginFile("TransformationPhasePlugin");
  const QString TransformationPhasePluginDisplayName("TransformationPhasePlugin");
  const QString TransformationPhaseBaseName("TransformationPhasePlugin");

  const DREAM3D_STRING Initiators("Initiators");
  const DREAM3D_STRING Propagators("Propagators");
  const DREAM3D_STRING BadActors("BadActors");

  static const float CSL[21][5] =
  {
	{3.0f, 60.0f, 1.0f, 1.0f, 1.0f},
	{5.0f, 36.87f, 1.0f, 0.0f, 0.0f},
	{7.0f, 38.21f, 1.0f, 1.0f, 0.0f},
	{9.0f, 38.94f, 1.0f, 1.0f, 0.0f},
	{11.0f, 50.48f, 1.0f, 1.0f, 0.0f},
	{13.0f, 22.62f, 1.0f, 0.0f, 0.0f},
	{13.0f, 27.8f, 1.0f, 1.0f, 1.0f},
	{15.0f, 48.19f, 2.0f, 1.0f, 0.0f},
	{17.0f, 28.07f, 1.0f, 0.0f, 0.0f},
	{17.0f, 61.93f, 2.0f, 2.0f, 1.0f},
	{19.0f, 26.53f, 1.0f, 1.0f, 0.0f},
	{19.0f, 46.83f, 1.0f, 1.0f, 1.0f},
	{21.0f, 21.79f, 1.0f, 1.0f, 1.0f},
	{21.0f, 44.4f, 2.0f, 1.0f, 1.0f},
	{23.0f, 40.45f, 3.0f, 1.0f, 1.0f},
	{25.0f, 16.25f, 1.0f, 0.0f, 0.0f},
	{25.0f, 51.68f, 3.0f, 3.0f, 1.0f},
	{27.0f, 31.58f, 1.0f, 1.0f, 0.0f},
	{27.0f, 35.42f, 2.0f, 1.0f, 0.0f},
	{29.0f, 43.61f, 1.0f, 0.0f, 0.0f},
	{29.0f, 46.39f, 2.0f, 2.0f, 1.0f}
  }
}
#endif