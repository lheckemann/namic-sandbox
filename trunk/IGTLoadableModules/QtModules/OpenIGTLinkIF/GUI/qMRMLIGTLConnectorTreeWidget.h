/*==============================================================================

  Program: 3D Slicer

  Copyright (c) 2010 Kitware Inc.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

  This file was originally developed by Julien Finet, Kitware Inc.
  and was partially funded by NIH grant 3P41RR013218-12S1

==============================================================================*/

#ifndef __qMRMLIGTLConnectorTreeWidget_h
#define __qMRMLIGTLConnectorTreeWidget_h

// Qt includes
#include <QTreeView>

#include "qSlicerOpenIGTLinkIFModuleExport.h"

//class qMRMLSceneModel;
//class qMRMLSortFilterProxyModel;
//class qMRMLIGTLConnectorTreeWidgetPrivate;
//class vtkMRMLNode;
class vtkMRMLScene;

// TODO: Rename to qMRMLSceneTreeWidget
class Q_SLICER_QTMODULES_OPENIGTLINKIF_EXPORT qMRMLConnectorTreeWidget : public QTreeView
{
  Q_OBJECT
  //Q_PROPERTY(QString sceneModelType READ sceneModelType WRITE setSceneModelType)
  //Q_PROPERTY(bool listenNodeModifiedEvent READ listenNodeModifiedEvent WRITE setListenNodeModifiedEvent)
  //Q_PROPERTY(QStringList nodeTypes READ nodeTypes WRITE setNodeTypes)
  //Q_PROPERTY(bool fitSizeToVisibleIndexes READ fitSizeToVisibleIndexes WRITE setFitSizeToVisibleIndexes)

public:
  qMRMLIGTLConnectorTreeWidget(QWidget *parent=0);
  virtual ~qMRMLIGTLConnectorTreeWidget();

  vtkMRMLScene* mrmlScene()const;

  /// Could be Transform, Displayable, ModelHierarchy or ""
  QString sceneModelType()const;

  /// \sa qMRMLSceneModel::setListenNodeModifiedEvent
  void setListenNodeModifiedEvent(bool listen);
  /// \sa qMRMLSceneModel::listenNodeModifiedEvent
  bool listenNodeModifiedEvent()const;

  /// Customize the model
  void setSceneModel(qMRMLSceneModel* newSceneModel, const QString& modelType);
  
  ///
  /// Set/Get node types to display in the list
  /// NodeTypes are the class names, i.e. vtkMRMLViewNode,
  /// vtkMRMLTransformNode
  QStringList nodeTypes()const;
  void setNodeTypes(const QStringList& nodeTypes);

  ///
  /// Retrieve the sortFilterProxyModel used to filter/sort
  /// the nodes.
  /// The returned value can't be null.
  qMRMLSortFilterProxyModel* sortFilterProxyModel()const;
  qMRMLSceneModel* sceneModel()const;

  ///
  /// When true, the tree widget resize itself so that
  /// it's sizeHint is right for the visible indexes
  bool fitSizeToVisibleIndexes()const;
  void setFitSizeToVisibleIndexes(bool);

  virtual QSize minimumSizeHint()const;
  virtual QSize sizeHint()const;

public slots:
  void setMRMLScene(vtkMRMLScene* scene);

  /// If the modelType doesn't match any known model, nothing
  /// will happen
  void setSceneModelType(const QString& modelType);

signals:
  void currentNodeChanged(vtkMRMLNode* node);

protected slots:
  virtual void onCurrentRowChanged(const QModelIndex& index);
  void onNumberOfVisibleIndexChanged();
  void deleteCurrentNode();
  
protected:
  QScopedPointer<qMRMLIGTLConnectorTreeWidgetPrivate> d_ptr;

  // reimplemented for performance issues
  virtual void updateGeometries();
  virtual void mousePressEvent(QMouseEvent* event);

private:
  Q_DECLARE_PRIVATE(qMRMLIGTLConnectorTreeWidget);
  Q_DISABLE_COPY(qMRMLIGTLConnectorTreeWidget);
};

#endif
