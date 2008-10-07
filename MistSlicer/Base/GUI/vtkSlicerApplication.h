// .NAME vtkSlicerApplication
// .SECTION Description
// Contains slicer's style, application and collection of associated guis.

#ifndef __vtkSlicerApplication_h
#define __vtkSlicerApplication_h

#include "vtkStringArray.h"
#include "vtkSlicerBaseGUIWin32Header.h"
#include "vtkKWApplication.h"
#include "vtkKWRegistryHelper.h" // really could have been avoided :(
#include "itkMutexLock.h"

#include "vtkSlicerApplicationGUI.h"

class vtkSlicerModuleGUI;
class vtkSlicerGUILayout;
class vtkSlicerTheme;
class vtkSlicerGUICollection;
//BTX
class DisplayMessageQueue;
//ETX

// Description:
// Contains slicer's style, application and collection of associated guis.
//
class VTK_SLICER_BASE_GUI_EXPORT vtkSlicerApplication : public vtkKWApplication
{
 public:
    vtkTypeRevisionMacro ( vtkSlicerApplication, vtkKWApplication );

    // Description:
    // This is a singleton pattern New.  There will only be ONE
    // reference to a vtkSlicerApplication object per process.  Clients that
    // call this must call Delete on the object so that the reference
    // counting will work.   The single instance will be unreferenced when
    // the program exits.
    static vtkSlicerApplication* New();

    // Description:
    // Get the singleton
    static vtkSlicerApplication* GetInstance();

    // Description:
    // Do one tcl event and enter the event loop, allowing the application
    // interface to actually run.
    // - override the virtual method from vtkKWApplication to add event broker
    virtual void DoOneTclEvent();

    // Description:
    // Get the layout, theme, GUI collection and main application GUI 
    vtkGetObjectMacro ( DefaultGeometry, vtkSlicerGUILayout );
    vtkGetObjectMacro ( SlicerTheme, vtkSlicerTheme );
    vtkGetObjectMacro ( ModuleGUICollection, vtkSlicerGUICollection );
    vtkGetObjectMacro ( ApplicationGUI, vtkSlicerApplicationGUI );
    vtkSetObjectMacro ( ApplicationGUI, vtkSlicerApplicationGUI );

    // Description:
    // This method collects GUIs added to Slicer.
    virtual void AddModuleGUI ( vtkSlicerModuleGUI *gui );
    virtual void RemoveModuleGUI ( vtkSlicerModuleGUI *gui );
    virtual vtkSlicerModuleGUI* GetModuleGUIByName ( const char *name );

    // Description:
    // These methods manage windows associated with the application
    virtual void CloseAllWindows ( ) ;
    
    // Description:
    // Sets application behavior.
    virtual void ConfigureApplication ( );
    virtual int StartApplication ( );

    // Description:
    // installs rules to specify look & feel.
    virtual void InstallTheme ( vtkKWTheme *theme );

  // Description:
  // methods to access registry from wrapped languages
  int HasRegistry(const char *key);
  void RequestRegistry(const char *key);
  const char *GetRegistryHolder();
  void SetRegistry(const char *key, char *value);

  // Description:
  // Save/Retrieve the application settings to/from registry.
  // Do not call that method before the application name is known and the
  // proper registry level set (if any).
  virtual void RestoreApplicationSettingsFromRegistry();
  virtual void SaveApplicationSettingsToRegistry();
  virtual void ConfigureRemoteIOSettingsFromRegistry();
  virtual void UpdateRemoteIOSettingsForRegistry();

  // Description:
  // Some constants
  //BTX
  static const char *ConfirmDeleteRegKey;
  static const char *ModulePathsRegKey;
  static const char *PotentialModulePathsRegKey;
  static const char *ModuleCachePathRegKey;
  static const char *TemporaryDirectoryRegKey;
  static const char *WebBrowserRegKey;
  static const char *UnzipRegKey;
  static const char *ZipRegKey;
  static const char *RmRegKey;
  static const char *HomeModuleRegKey;
  static const char *LoadModulesRegKey;
  static const char *IgnoreModulesRegKey;
  static const char *LoadCommandLineModulesRegKey;
  static const char *EnableDaemonRegKey;
  static const char *ApplicationFontFamilyRegKey;
  static const char *ApplicationFontSizeRegKey;
  static const char *ApplicationWindowWidthRegKey;
  static const char *ApplicationWindowHeightRegKey;
  static const char *ApplicationSlicesFrameHeightRegKey;
  static const char *ApplicationLayoutTypeRegKey;
  static const char *EnableAsynchronousIORegKey;
  static const char *EnableForceRedownloadRegKey;
  static const char *EnableRemoteCacheOverwritingRegKey;
  static const char *RemoteCacheDirectoryRegKey;
  static const char *RemoteCacheLimitRegKey;
  static const char *RemoteCacheFreeBufferSizeRegKey;
  //ETX


  // Description:
  // Set/Get the application font family
  void SetApplicationFontFamily ( const char *family);
  const char *GetApplicationFontFamily ( ) const;

  // Description:
  // Set/Get the application font size
  void SetApplicationFontSize ( const char *size );
  const char *GetApplicationFontSize ( ) const;

  // Descrition:
  // Set/Get the confirm delete flag
  void SetConfirmDelete(const char* state);
  const char* GetConfirmDelete() const;
  
  // Description:
  // Set/Get the search paths for modules.
  // This is a list of paths delimited by a specific seperator: ';' on 
  // Windows, ':' on Unix/MacOSX platforms.
  void SetModulePaths(const char *paths);
  const char* GetModulePaths() const;

  // Description:
  // Set/Get the potential search paths for modules.
  // This is a list of directories that can be used as module paths. 
  // Each item in this list is a directory and a boolean flag (0 or 1) 
  // specifying if that directory is actually to be used as a module path
  // (see ModulePaths, which is the subset of the paths in PotentialModulePaths
  // that are enabled, with a different delimiter between each path). 
  // This variable is used for GUI purposes, in that it lets people keep a
  // list of directories and enable/disable them at will, without having
  // to re-enter/re-pick them one by one using a file browser. It is used
  // by the vtkSlicerApplicationSettingsInterface and computed with help its
  // vtkKWDirectoryPresetSelector internal class. Each element is separated by
  // a '|' delimiter (ex: "c:/temp|0|d:/foo/bar|1", where "c:/temp" is disabled
  // and d:/foo/bar is enabled; at this point, the value of ModulePaths should
  // actually be "d:/foo/bar").
  void SetPotentialModulePaths(const char *paths);
  const char* GetPotentialModulePaths() const;

  // Description:
  // Set/Get the cache path for modules.
  void SetModuleCachePath(const char *path);
  const char* GetModuleCachePath() const;
  
  // Description:
  // Set/Get a user's home module.
  void SetHomeModule (const char *name);
  const char *GetHomeModule() const;
  
  // Description:
  // Set/Get an executable firefox browser for modules that need one.
  void SetWebBrowser ( const char *browser);
  const char* GetWebBrowser () const;

  // Description:
  // Set/Get an executable zip and unzip for modules that need one
  void SetUnzip ( const char *unzip );
  const char *GetUnzip() const;
  void SetZip (const char *zip);
  const char *GetZip() const;

  // Description:
  // Set/Get an executable rm for modules that need one
  void SetRm ( const char *rm );
  const char *GetRm() const;
  
  // Description:
  // Set/Get a directory for temporary file storage
  void SetTemporaryDirectory(const char *path);
  const char* GetTemporaryDirectory() const;

  // Description:
  // Set/Get a directory for the remote file cache
  void SetRemoteCacheDirectory(const char *path);
  const char* GetRemoteCacheDirectory() const;
  
  // Description:
  // Set/Get the application window size
  // for saving in the registry.
  void SetApplicationWindowSize (int width, int height );
  vtkGetMacro (ApplicationWindowWidth, int);
  vtkSetMacro (ApplicationWindowWidth, int);
  vtkGetMacro (ApplicationWindowHeight, int);
  vtkSetMacro (ApplicationWindowHeight, int);

  // Description:
  // Saves the application window size for registry setting
  void SaveApplicationWindowConfiguration ( );

  // Description:
  // Set/Get the application layout for saving in
  // the registry.
  vtkGetMacro (ApplicationSlicesFrameHeight, int );
  vtkSetMacro (ApplicationSlicesFrameHeight, int );
  vtkGetMacro (ApplicationLayoutType, int );
  vtkSetMacro (ApplicationLayoutType, int );

  // Description:
  // Set/Get if modules should be loaded (i.e. loadable modules)
  vtkSetMacro(LoadModules, int);
  vtkGetMacro(LoadModules, int);
  vtkBooleanMacro(LoadModules, int);

  // Description:
  // Set/Get names of modules that should be ignored (of all loadable modules)
  vtkSetObjectMacro(IgnoreModules, vtkStringArray);
  vtkGetObjectMacro(IgnoreModules, vtkStringArray);

  // Description:
  // Set/Get names of all loadable modules
  vtkSetObjectMacro(LoadableModules, vtkStringArray);
  vtkGetObjectMacro(LoadableModules, vtkStringArray);


  // Description:
  // Set/Get if command line modules should be loaded (i.e. CLI plugins)
  vtkSetMacro(LoadCommandLineModules, int);
  vtkGetMacro(LoadCommandLineModules, int);
  vtkBooleanMacro(LoadCommandLineModules, int);

  // Description:
  // Set/Get if the slicer daemon should be loaded
  vtkSetMacro(EnableDaemon, int);
  vtkGetMacro(EnableDaemon, int);
  vtkBooleanMacro(EnableDaemon, int);

  // Description:
  // Set/Get if asynch IO should be used
  void SetEnableAsynchronousIO ( int );
  vtkGetMacro(EnableAsynchronousIO, int);
  vtkBooleanMacro(EnableAsynchronousIO, int);

  // Description:
  // Set/Get if re-downloads to the cache should be forced
  void SetEnableForceRedownload (int);
  vtkGetMacro(EnableForceRedownload, int);
  vtkBooleanMacro(EnableForceRedownload, int);

  // Description:
  // Set/Get if should force overwriting cache files
  void SetEnableRemoteCacheOverwriting (int);
  vtkGetMacro(EnableRemoteCacheOverwriting, int);
  vtkBooleanMacro(EnableRemoteCacheOverwriting, int);

  // Description:
  // Control the remote cache directory size, in Mb
  void SetRemoteCacheLimit ( int);
  vtkGetMacro (RemoteCacheLimit, int);

  // Description:
  // Control the remote cache directory free buffer size, in Mb
  void SetRemoteCacheFreeBufferSize ( int );
  vtkGetMacro (RemoteCacheFreeBufferSize, int);  

  // Description:
  // Evaluate a string as a tcl expression
  const char *Evaluate(const char *expression);

  // Description:
  // Request that a message be placed in the logger
  // widget. RequestDisplayMessage() allows records to be added to the
  // logger widget from a separate thread. First argument is the type
  // of message ("Error", "Warning", "Information", "Debug").  Second
  // argument is the message to display.
  bool RequestDisplayMessage( const char *type, const char* message );
  
  // Description:
  // Process a request to place a message on the log widget. This
  // method is called in the main thread of the application because
  // calls to the log widget can cause an update to the GUI. (Method
  // needs to be public to fit in the event callback chain.)
  void ProcessDisplayMessage();

  // Description:
  // Put a message in the logger widget.  These methods actually
  // schedule the message display so that only the application
  // associated with the user interface attempts to display the
  // message. THese methods delegate to RequestDisplayMessage().
  virtual void WarningMessage(const char* message);
  virtual void ErrorMessage(const char* message);
  virtual void DebugMessage(const char* message);
  virtual void InformationMessage(const char* message);
  
  // Description:
  // Override the KWWidgets default behavior of setting the 'transient'
  // flag on the interactor, which prevents it from being hidden
  // by the main application window
  virtual void DisplayTclInteractor(vtkKWTopLevel *master);
  virtual void DisplayLogDialog(vtkKWTopLevel *master);

//BTX
  // Pass/cast PyObject* in place of void*
  virtual void InitializePython(void* mod, void* dict);
  virtual void* GetPythonModule();
  virtual void* GetPythonDictionary();
//ETX

  // Description:
  // Add additional copyright messages
  virtual void AddAboutCopyrights(ostream &);

  // Description:
  // Control the state of the splash screen
  vtkSetMacro (UseSplashScreen, int);
  vtkGetMacro (UseSplashScreen, int);

  // Description:
  // Control stereo render capability
  vtkSetMacro (StereoEnabled, int);
  vtkGetMacro (StereoEnabled, int);
  
  // Description:
  // Control the state of the splash screen
  void SplashMessage (const char * message);

  // Description:
  // Entry point to the interpreter that can be used 
  // by an external application tro trigger
  // performance analysis
  const char *TraceScript (const char * script) {return (this->Script(script));};

 protected:
  vtkSlicerApplication ( );
  virtual ~vtkSlicerApplication ( );

  // Description:
  // Contains the default geometry of the ApplicationGUI
  vtkSlicerGUILayout *DefaultGeometry;
  // Description:
  // Contains the description for Slicer GUI's look and feel
  vtkSlicerTheme *SlicerTheme;
  // Description:
  // The main application GUI.
  vtkSlicerApplicationGUI *ApplicationGUI;
  // Description:
  // Collections of GUIs
  vtkSlicerGUICollection *ModuleGUICollection;

//BTX
  void StringToArray(std::string string, char separator, vtkStringArray *array);
  void ArrayToString(vtkStringArray *array, std::string sep, char *string, int maxLength );
//ETX  

  char ConfirmDelete[vtkKWRegistryHelper::RegistryKeyValueSizeMax];
  char ModulePaths[vtkKWRegistryHelper::RegistryKeyValueSizeMax];
  char PotentialModulePaths[vtkKWRegistryHelper::RegistryKeyValueSizeMax];
  char ModuleCachePath[vtkKWRegistryHelper::RegistryKeyValueSizeMax];
  char WebBrowser [vtkKWRegistryHelper::RegistryKeyValueSizeMax ];
  char Unzip [vtkKWRegistryHelper::RegistryKeyValueSizeMax ];
  char Zip [vtkKWRegistryHelper::RegistryKeyValueSizeMax ];
  char Rm [vtkKWRegistryHelper::RegistryKeyValueSizeMax ];
  char TemporaryDirectory[vtkKWRegistryHelper::RegistryKeyValueSizeMax];
  char HomeModule [ vtkKWRegistryHelper::RegistryKeyValueSizeMax];
  char ApplicationFontSize [vtkKWRegistryHelper::RegistryKeyValueSizeMax];
  char ApplicationFontFamily [vtkKWRegistryHelper::RegistryKeyValueSizeMax];
  char IgnoreModuleNames [vtkKWRegistryHelper::RegistryKeyValueSizeMax];
  
  int ApplicationWindowWidth;
  int ApplicationWindowHeight;
  int ApplicationSlicesFrameHeight;
  int ApplicationLayoutType;

  char RegistryHolder [vtkKWRegistryHelper::RegistryKeyValueSizeMax];

  vtkStringArray *IgnoreModules;
  vtkStringArray *LoadableModules;

  int LoadModules;
  int LoadCommandLineModules;
  int EnableDaemon;
  void* PythonModule;
  void* PythonDictionary;

  int EnableAsynchronousIO;
  int EnableForceRedownload;
  int EnableRemoteCacheOverwriting;
  char RemoteCacheDirectory[vtkKWRegistryHelper::RegistryKeyValueSizeMax];
  int RemoteCacheLimit;
  int RemoteCacheFreeBufferSize;

  
private:
  vtkSlicerApplication ( const vtkSlicerApplication& ); // Not implemented.
  void operator = ( const vtkSlicerApplication& ); //Not implemented.
    
  //BTX
  itk::MutexLock::Pointer DisplayMessageQueueActiveLock;
  itk::MutexLock::Pointer DisplayMessageQueueLock;
  std::string NameSeparator;
  //ETX
  bool DisplayMessageQueueActive;

  DisplayMessageQueue* InternalDisplayMessageQueue;
  
  static vtkSlicerApplication* Instance;

  int UseSplashScreen;
  int StereoEnabled;
}; 

#endif
