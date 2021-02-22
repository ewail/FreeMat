function vren1(volume,opacity,ctrans,matlprop)
scalars = vtkShortArray;
scalars.SetArray(volume(:),numel(volume),0);
image_data = vtkImageData;
pd = image_data.GetPointData();
pd.SetScalars(scalars);
image_data.SetDimensions(size(volume));
%image_data.SetNumberOfScalarComponents(1);
image_data.SetScalarTypeToShort();
opacityTransferFunction = vtkPiecewiseFunction;
for i=1:size(opacity,2)
  opacityTransferFunction.AddPoint(opacity(1,i),opacity(2,i));
end
colorTransferFunction = vtkColorTransferFunction;
for i=1:size(ctrans,2)
  colorTransferFunction.AddHSVPoint(ctrans(1,i),ctrans(2,i),ctrans(3,i),ctrans(4,i));
end
volumeProperty = vtkVolumeProperty;
volumeProperty.SetColor(colorTransferFunction);
volumeProperty.SetScalarOpacity(opacityTransferFunction);
volumeProperty.ShadeOn();
volumeProperty.SetDiffuse(matlprop(1));
volumeProperty.SetAmbient(matlprop(2));
volumeProperty.SetSpecular(matlprop(3));
volumeProperty.SetSpecularPower(matlprop(4));
volumeProperty.SetInterpolationTypeToLinear();
compositeFunction = vtkVolumeRayCastCompositeFunction;
mapper = vtkFixedPointVolumeRayCastMapper;
mapper.SetInput(image_data);
actor = vtkVolume;
actor.SetMapper(mapper);
actor.SetProperty(volumeProperty);
ren = vtkRenderer;
ren.SetBackground(1,1,1);
ren.AddVolume(actor);
ren.ResetCamera();
vtkfigure(ren);

