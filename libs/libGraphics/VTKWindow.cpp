
#ifdef HAVE_VTK
#include "VTKWrap.hpp"
#include "VTKWindow.hpp"

#include <QtCore/QString>
#include <QtGui/QLabel>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QAction>
#include <QtGui/QFrame>
#include <QtGui/QComboBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QCompleter>
#include <QtGui/QFileSystemModel>
#include <QtGui/QPushButton>
#include <QtGui/QSplitter>
#include <QtGui/QGroupBox>

#include <QtCore/QDebug>

#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include "vtkCylinderSource.h"
#include <vtkPolyDataMapper.h>
#include <vtkStructuredPointsReader.h>
#include <vtkStructuredPoints.h>
#include <vtkVolumeRayCastMapper.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>
#include <vtkVolumeProperty.h>
#include <vtkVolumeRayCastCompositeFunction.h>
#include <vtkVolumeTextureMapper3D.h>
#include <vtkOpenGLVolumeTextureMapper3D.h>
#include <vtkFixedPointVolumeRayCastMapper.h>
#include <vtkImagePlaneWidget.h>
#include <vtkActor2D.h>
#include <vtkOpenGLImageMapper.h>
#include <vtkImageReslice.h>
#include <vtkPNGWriter.h>
#include <vtkProperty.h>
#include <vtkImageImport.h>

void VTKViewer::LoadData(const Array &dp, 
			   const Array &Opacity,
			   const Array &ColorTransfer,
			   const Array &Material) {
  source = vtkImageImport::New();
  volume = dp.asDenseArray().toClass(UInt16);
  
  source->SetImportVoidPointer(volume.getVoidPointer());
  source->SetDataScalarTypeToUnsignedShort();
  source->SetDataExtent(0,volume.dimensions()[0]-1,
			0,volume.dimensions()[1]-1,
			0,volume.dimensions()[2]-1);

  // Create transfer mapping scalar value to opacity
  vtkPiecewiseFunction* opacityTransferFunction = vtkPiecewiseFunction::New();
  const BasicArray<double>& opt = Opacity.constReal<double>();
  for (int i=0;i<opt.cols();i++)
    opacityTransferFunction->AddPoint(opt[NTuple(1,i+1)],opt[NTuple(2,i+1)]);

  // Create transfer mapping scalar value to color
  vtkColorTransferFunction* colorTransferFunction = vtkColorTransferFunction::New();
  const BasicArray<double> &cpt = ColorTransfer.constReal<double>();
  for (int i=0;i<cpt.cols();i++)
    colorTransferFunction->AddHSVPoint(cpt[NTuple(1,i+1)],
				       cpt[NTuple(2,i+1)],
				       cpt[NTuple(3,i+1)],
				       cpt[NTuple(4,i+1)]);

  // The property describes how the data will look
  vtkVolumeProperty* volumeProperty = vtkVolumeProperty::New();
  volumeProperty->SetColor ( colorTransferFunction );
  volumeProperty->SetScalarOpacity ( opacityTransferFunction );
  volumeProperty->ShadeOn();
  const BasicArray<double> &mpt = Material.constReal<double>();
  volumeProperty->SetDiffuse ( mpt[1] );
  volumeProperty->SetAmbient ( mpt[2] );
  volumeProperty->SetSpecular ( mpt[3] );
  volumeProperty->SetSpecularPower ( mpt[4] );
  volumeProperty->SetInterpolationTypeToLinear();

  // The mapper / ray cast function know how to render the data
  vtkVolumeRayCastCompositeFunction* compositeFunction = vtkVolumeRayCastCompositeFunction::New();

  // Mapper
  //  mapper = vtkOpenGLVolumeTextureMapper3D::New();
  mapper = vtkFixedPointVolumeRayCastMapper::New();

  //mapper->ImmediateModeRenderingOn();
  //mapper->SetVolumeRayCastFunction ( compositeFunction );
  mapper->SetInputConnection ( source->GetOutputPort() );

  source->Update();
  qDebug() << source->GetOutput()->GetDimensions() [0];
  qDebug() << source->GetOutput()->GetDimensions() [2];
  // Actor in scene
  actor = vtkVolume::New();
  actor->SetMapper ( mapper );
  actor->SetProperty ( volumeProperty );

  // Add Actor to renderer
  ren->AddVolume ( actor );
	
  // Reset camera
  ren->ResetCamera();
  ren->GetRenderWindow()->Render();
}

VTKViewer::VTKViewer() {
  actor = NULL;

  QVBoxLayout *layout = new QVBoxLayout;

  // QT/VTK interact
  vtkWidget = new QVTKWidget ( this );
  vtkWidget->setMinimumSize ( 400, 400 );
  ren = vtkRenderer::New();
  ren->SetBackground ( 1, 1, 1 );
  vtkWidget->GetRenderWindow()->AddRenderer ( ren );

  layout->addWidget(vtkWidget);
  setLayout(layout);

  source = NULL;
  mapper = NULL;
  actor = NULL;
  
}

VTKViewer::~VTKViewer() {
  ren->Delete();

  if ( source ) {
    source->Delete();
  }

  if ( mapper ) {
    mapper->Delete();
  }

  if ( actor ) {
    actor->Delete();
  }
}

// Help for volrender
//
// volrender(volume, opacityfunction, colortransferfunction, matlpropvec)

//@@Signature
//sgfunction vtkfigure VTKFigureFunction
//input render
//output none
ArrayVector VTKFigureFunction(int nargout, const ArrayVector& arg, Interpreter *)
{
  if (arg.size() != 1) return ArrayVector();
  vtkRenderer *r = GetVTKPointer<vtkRenderer>(arg[0]);
  QVTKWidget *p = new QVTKWidget;
  p->setMinimumSize(400,400);
  p->GetRenderWindow()->AddRenderer(r);
  p->show();
  return ArrayVector();
}

//@@Signature
//sgfunction volrender VolRenderFunction
//input volume opacity ctransfer material
//output none
ArrayVector VolRenderFunction(int nargout, const ArrayVector& arg, Interpreter *) 
{
  if (arg.size() < 4)
    throw Exception("volrender requires at least 4 arguments (volume, opacity, colortransfer, material)");
  // Validate the data
  if (arg[0].isReferenceType())
    throw Exception("volrender cannot accept non-numeric volumes");
  if (!arg[0].allReal())
    throw Exception("volrender dataset cannot be complex");
  Array Opacity = arg[1].asDenseArray().toClass(Double);
  if (!Opacity.is2D() || Opacity.rows() != 2)
    throw Exception("opacity argument must be a 2 X N array");
  Array ColorTransfer = arg[2].asDenseArray().toClass(Double);
  if (!ColorTransfer.is2D() || ColorTransfer.rows() != 4)
    throw Exception("color transfer function must be a 4 X N array");
  Array Material = arg[3].asDenseArray().toClass(Double);
  if (Material.length() != 4)
    throw Exception("material properties vector must be a length 4 vector");
  VTKViewer *foo = new VTKViewer();
  foo->LoadData(arg[0],Opacity,ColorTransfer,Material);
  foo->show();
  return ArrayVector();
}

//**********************************************************************


//#include "/home/sbasu/Devel/fm4/tools/vtkwrap/tst.c"

//**********************************************************************


#else

#include "Array.hpp"

ArrayVector VolRenderFunction(int nargout, const ArrayVector& arg, Interpreter *) 
{
  throw Exception("volrender function requires VTK support, which was not enabled at compile time");
}


ArrayVector VTKFigureFunction(int nargout, const ArrayVector& arg, Interpreter *)
{
  throw Exception("vtkfigure function requries VTK support, which was not enabled at compile time");
}

#endif
