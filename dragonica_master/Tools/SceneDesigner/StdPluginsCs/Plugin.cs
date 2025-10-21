using System;
using System.Collections;
using System.Diagnostics;
using System.IO;
using System.Reflection;
using System.Xml.Schema;
using System.ComponentModel;
using System.Windows.Forms;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;
using Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Editors;
using Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Panels;
using Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Services;
using Emergent.Gamebryo.SceneDesigner.StdPluginsCs.UIBinding;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs
{
	/// <summary>
	/// Summary description for Plugin.
	/// </summary>
	public class Plugin : IPlugin
	{
        #region Private Data
        private static readonly string m_menuFileExtension = "menu.xml";
        private static readonly string m_toolbarFileExtension = "toolbar.xml";
	    private static readonly string m_commandFileExtension = "commands.xml";
	    private static readonly string m_shortcutFileExtension 
            = "shortcuts.xml";
        private static readonly string m_optionsBindingFileExtension
            = "optbind.xml";
        private static readonly string ms_strPathSettingName =
            "External Application Path";
        private static readonly string ms_strCommandLineSettingName =
            "External Application Command Line";
	    #endregion

        public Plugin()
		{
            
		}

	    public string Name
	    {
	        get { throw new NotImplementedException(); }
	    }

	    public Version Version
	    {
	        get
	        { return Assembly.GetExecutingAssembly().GetName().Version; }
	    }

	    public Version ExpectedVersion
	    {
	        get 
            { return new Version(1, 0); }
	    }

	    /// <summary>
	    /// Called when a Plugin is loaded
	    /// </summary>
	    /// <param name="toolMajorVersion"></param>
	    /// <param name="toolMinorVersion"></param>
	    public void Load(int toolMajorVersion, int toolMinorVersion)
	    {
	        
	    }

	    /// <summary>
	    /// Provides a mechanism to allow packages to report services
	    /// </summary>
	    /// <returns>A list of service objects that this package provides</returns>
	    public IService[] GetProvidedServices()
	    {
            IService[] retVal = 
                new IService[] {new UICommandServiceImpl(),
                                new OptionsServiceImpl(),
                                new EntityPathServiceImpl()};

            UICommandUtility.Init();
	        return retVal;
	    }

	    /// <summary>
	    /// Provides a mechanism for a plugin to interact with services.
	    /// </summary>
	    public void Start()
	    {
            
            ServiceProvider serviceProvider = 
                ServiceProvider.Instance;

            LoadPanels();
            string searchFolderRoot = MFramework.Instance.AppStartupPath;
            string[] folders = BuildSubfolderList(searchFolderRoot);

            BuildItems(serviceProvider, 
                new DynamicMenuBuilder(serviceProvider), 
                BuildFileList(folders, m_menuFileExtension),
                BuildSchemaPath(m_menuFileExtension));

            BuildItems(serviceProvider, 
                new DynamicToolbarBuilder(serviceProvider),
                BuildFileList(folders, m_toolbarFileExtension),
                BuildSchemaPath(m_toolbarFileExtension));

            BuildItems(serviceProvider, 
                new DynamicShortcutBuilder(serviceProvider),
                BuildFileList(folders, m_shortcutFileExtension),
                BuildSchemaPath(m_shortcutFileExtension));

            CommandBindingBuilder bindingBuilder = 
                new CommandBindingBuilder(serviceProvider);

            BuildItems(serviceProvider, bindingBuilder,
                BuildFileList(folders, m_commandFileExtension),
                null);

            BuildItems(serviceProvider, new OptionsBindingsBuilder(),
                BuildFileList(folders, m_optionsBindingFileExtension),
                BuildSchemaPath(m_optionsBindingFileExtension));

            bindingBuilder.BuildFromLoadedAssemblies(); 

            RegisterPropertyTypes();

            RegisterExternalApplicationSettings();
			
			ICommandPanelService panelService =
				serviceProvider.GetService(typeof(ICommandPanelService))
				as ICommandPanelService;
		}


	    private void LoadPanels()
	    {
            ServiceProvider serviceProvider = 
                ServiceProvider.Instance;
	        ICommandPanelService panelService = 
                serviceProvider.GetService(typeof(ICommandPanelService))
                as ICommandPanelService;

            IMessageService messageService = 
                serviceProvider.GetService(typeof(IMessageService))
                as IMessageService;

            IUICommandService commandService =
                serviceProvider.GetService(typeof(IUICommandService))
                as IUICommandService;
           
            panelService.RegisterPanel(new MessagesPanel(commandService,
                messageService));

            panelService.RegisterPanel(new UndoPanel());

            panelService.RegisterPanel(new ComponentPanel());

            panelService.RegisterPanel(new EntityTemplatePanel());

            panelService.RegisterPanel(new PalettePanel());

            panelService.RegisterPanel(new RoomPanel());

	    }

        private void RegisterPropertyTypes()
        {
            IPropertyTypeService pmPropertyTypeService =
                ServiceProvider.Instance.GetService(
                typeof(IPropertyTypeService)) as IPropertyTypeService;
            Debug.Assert(pmPropertyTypeService != null, "Property type " +
                "service not found!");

            pmPropertyTypeService.RegisterType(new PropertyType(
                "Entity Pointer", "Entity Pointer", typeof(MEntity),
                typeof(EntityPointerEditor), null,
                typeof(EntityCollectionEditor)));
        }

        private void RegisterExternalApplicationSettings()
        {
            ISettingsService pmSettingsService = ServiceProvider.Instance
                .GetService(typeof(ISettingsService)) as ISettingsService;

            string strSceneAppPath = Application.ExecutablePath;
            int iIndex = strSceneAppPath.LastIndexOf('\\');
            for (int iCount = 0; iIndex > -1 && iCount < 6; iCount++)
            {
                strSceneAppPath = strSceneAppPath.Substring(0, iIndex);
                iIndex = strSceneAppPath.LastIndexOf('\\');
            }
            strSceneAppPath += "\\Samples\\FullDemos\\SceneApp\\Win32\\" +
#if VC71
                "VC71"
#elif VC80
                "VC80"
#endif
                + "\\SceneApp.exe";
            pmSettingsService.RegisterSettingsObject(ms_strPathSettingName,
                new MFileLocation(strSceneAppPath), SettingsCategory.PerUser);

            pmSettingsService.RegisterSettingsObject(
                ms_strCommandLineSettingName, string.Empty,
                SettingsCategory.PerUser);
        }

        #region UI Command Binding Helpers
        private void BuildItems(ServiceProvider provider, 
            AbstractBuilder builder, FileInfo[] files, string schemaPath
            )
        {
            IMessageService messageService = 
                provider.GetService(typeof(IMessageService))
                as IMessageService;

            XmlSchema schema = null;
            if (schemaPath != null)
            {
                try
                {

                    using (FileStream schemaStream = new FileStream(schemaPath,
                               FileMode.Open, FileAccess.Read))
                    {
                        schema = XmlSchema.Read(schemaStream, null);
                    }
                }
                catch (FileNotFoundException)
                {
                    messageService.AddMessage(MessageChannelType.General,
                        string.Format("Schema '{0}' not found",
                        schemaPath));
                }
            }

            builder.Schema = schema;

            foreach(FileInfo file in files)
            {
                try
                {
                    using (Stream stream = 
                               new FileStream(file.FullName, 
                               FileMode.Open, FileAccess.Read))
                    {
                        builder.Build(stream);
                    }
                }
                catch (Exception e)
                {        
                    PluginAPI.Message message = new PluginAPI.Message();
                    message.m_strText = "Could not load file: " +
                        Environment.NewLine + file.FullName;
                    message.m_strDetails = e.ToString();
                    messageService.AddMessage(MessageChannelType.Errors,
                        message);
                }
            }
        }

        private string[] BuildSubfolderList(string rootPath)
        {
            ArrayList directoryNames = new ArrayList();
            DirectoryInfo rootInfo = new DirectoryInfo(rootPath);
            directoryNames.Add(rootInfo.FullName);
            DirectoryInfo[] subFolders = rootInfo.GetDirectories();
            foreach (DirectoryInfo di in subFolders)
            {
                directoryNames.AddRange(BuildSubfolderList(di.FullName));
            }           
            return (string[]) directoryNames.ToArray(typeof(string));
        }

        private FileInfo[] BuildFileList(string[] folders, string extension)
        {
            ArrayList fileInfoList = new ArrayList();
            foreach (string folder in folders)
            {
                DirectoryInfo di = new DirectoryInfo(folder);
                FileInfo[] files = di.GetFiles("*." + extension);
                fileInfoList.AddRange(files);
            }
            return (FileInfo[]) fileInfoList.ToArray(typeof(FileInfo));
        }

        private string BuildSchemaPath(string extension)
        {
            return MFramework.Instance.AppStartupPath + 
                @"data\" + extension.Replace("xml", "xsd");
        }
        #endregion

        #region UI Command Handlers
        [UICommandHandler("CreateCameraFromViewport")]
        private static void OnCreateCameraFromViewport(object sender,
            EventArgs args)
        {
            // Take the current viewport camera and clone it
            MEntity currentCamera = MFramework.Instance.ViewportManager
                .ActiveViewport.CameraEntity;

            String cloneName = currentCamera.Name + " 01";
            cloneName = MFramework.Instance.Scene.GetUniqueEntityName(
                cloneName);
            MEntity clonedCamera = currentCamera.Clone(cloneName, false);
			clonedCamera.PGProperty = (MEntity.ePGProperty)Enum.Parse(typeof(MEntity.ePGProperty), "MinimapCamera");

            // Add it to the scene
            MFramework.Instance.Scene.AddEntity(clonedCamera, true);

            // Select the current camera, de-selecting any current selections
            ISelectionService selectionService =
                ServiceProvider.Instance.GetService(typeof(ISelectionService))
                as ISelectionService;

            selectionService.ClearSelectedEntities();
            selectionService.AddEntityToSelection(clonedCamera);

            // Set the viewport to use the newly cloned camera
            MFramework.Instance.ViewportManager.ActiveViewport.SetCamera(
                clonedCamera, false);
        }

        [UICommandHandler("LaunchExternalApplication")]
        private static void OnLanchExternalApplication(object sender,
            EventArgs args)
        {
            ISettingsService settingService =
                ServiceProvider.Instance.GetService(typeof(ISettingsService))
                as ISettingsService;

            MFileLocation pmFileLocation = (MFileLocation)
                settingService.GetSettingsObject(ms_strPathSettingName,
                SettingsCategory.PerUser);
            if (pmFileLocation == null ||
                pmFileLocation.Path.Equals(string.Empty))
            {
                MessageBox.Show("Cannot launch external application " +
                    "because no path has been set.\nCheck your \"" +
                    ms_strPathSettingName + "\" option and try again.",
                    "External Application Path Not Set", MessageBoxButtons.OK,
                    MessageBoxIcon.Error);
                return;
            }
            string strAppPath = pmFileLocation.Path;

            string strCommandLine = (string)settingService.GetSettingsObject(
               ms_strCommandLineSettingName, SettingsCategory.PerUser);
            if (strCommandLine == null)
            {
                strCommandLine = string.Empty;
            }

            string strPath = Path.GetTempPath() + "Temp.gsa";
            if (!MFramework.Instance.SaveScene(strPath, "GSA", false))
            {
                MessageBox.Show("Cannot save temporary scene file to the " +
                    "following location:\n\n" + strPath + "\n\nUnable to " +
                    "launch external application.", "Cannot Save Temporary " +
                    "Scene File", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
            string strFilename = "\"" + strPath + "\"";
            string strFullCommandLine = " " +
                strFilename + " " + strCommandLine;

            bool bLaunched = false;
            try
            {
                Process myProcess = new Process();
                myProcess.StartInfo.Arguments = strFullCommandLine;
                myProcess.StartInfo.FileName = "\"" + strAppPath + "\"";
                bLaunched = myProcess.Start();

            }
            catch (System.Exception)
            {
            }

            if (!bLaunched)
            {
                MessageBox.Show("Cannot launch external application from " +
                    "the following location:\n\n" + strAppPath +
                    "\n\nwith the following command-line parameters:\n\n" +
                    strFullCommandLine + "\n\nCheck your \"" +
                    ms_strPathSettingName + "\" and \"" +
                    ms_strCommandLineSettingName + "\" options.",
                    "External Application Launch Failure",
                    MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }
        }
        #endregion

        [DllInit]
        private static void Init()
        {
            
        }

        [DllShutdown]
        private static void Shutdown()
        {
            
        }
	}
}
