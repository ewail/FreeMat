#ifndef __VTKWindow_HPP__
#define __VTKWindow_HPP__

#ifdef HAVE_VTK
#include "Array.hpp"

#include <QtGui/QMainWindow>
#include <QtGui/QComboBox>
#include <QtGui/QSlider>

#include <QVTKWidget.h>

// Forward class declarations
class vtkStructuredPointsReader;
class vtkImageImport;
class vtkPolyDataMapper;
class vtkVolumeMapper;
class vtkImagePlaneWidget;
class vtkImageViewer2;

class vtkVolume;
class vtkRenderer;

class VTKViewer : public QWidget {
  Q_OBJECT
private:
  QComboBox* fileNameComboBox;
  QString fileName;
  
  vtkImageImport* source;
  vtkVolumeMapper* mapper;
  vtkVolume* actor;
  vtkRenderer* ren;
  QVTKWidget* vtkWidget;
  Array volume;
public:
  VTKViewer();
  void LoadData(const Array &data, const Array &Opacity, const Array &ColorTransfer, const Array &Material);
  virtual ~VTKViewer();
};

#endif

#endif
