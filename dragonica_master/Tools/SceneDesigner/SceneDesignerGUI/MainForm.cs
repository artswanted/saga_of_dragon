// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//	  Copyright (c) 1996-2006 Emergent Game Technologies.
//	  All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

using System;
using System.Collections;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Windows.Forms;
using System.Xml;
using System.Globalization;
using Microsoft.VisualBasic.FileIO;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.GUI.PluginCore;
using Emergent.Gamebryo.SceneDesigner.GUI.PluginCore.StandardServices;
using Emergent.Gamebryo.SceneDesigner.GUI.Utility;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;
using Emergent.Gamebryo.SceneDesigner.GUI.ProjectG;
using Message = Emergent.Gamebryo.SceneDesigner.PluginAPI.Message;

namespace Emergent.Gamebryo.SceneDesigner.GUI
{
	/// <summary>
	/// Summary description for MainForm.
	/// </summary>
	public class MainForm : System.Windows.Forms.Form
	{
		private ShortcutServiceImpl m_shortcutService;
		private System.ComponentModel.IContainer components;
		private TimedInvoker m_timer;
		IUICommandService m_uiCommandService;
		UICommand m_idleCommand;
		private static readonly string dockConfigSettingName = 
			"DockConfig";
		private System.Windows.Forms.Timer m_timerComponent;
		private static readonly string m_paletteFileExtension = "pal";
		private bool m_lastDirtyStatus;
		private string m_currentFileName;
		private DateTime m_timeOfLastAutoSave;
		private bool m_bClearWindowLayout = false;
		private bool m_bCustomUILoaded = false;
		static private Mutex m_restartLock = null;
		/// <summary>
		/// Only one instace of Scene Designer is allowed.
		/// </summary>
		static private Mutex m_instanceLock;

		// Todo : Xml ������
		private XmlManager m_kXmlManager = new XmlManager();
		private TriggerPanel m_kTriggerPanel;
		private MonsterPanel m_kMonsterPanel;
		private PathPanel m_kPathPanel;
		private NpcPanel m_kNpcPanel;
		private WayPointPanel m_kWayPointPanel;
		private MonsterArea m_kMonsterAreaPanel;
		private XmlPropertiesPanel m_kXmlPropertiesPanel;
		private NotExistFilesPanel m_kNotExistFilesPanel;
		private StonePanel m_kStonePanel;
        //private DecalPanel m_kDecalPanel;
        private BreakObjectPanel m_kBreakObjectPanel;
        private ElevatorPanel m_kElevatorPanel;
		private FogPanel m_kFogPanel;
		private ZoneControl m_kZoneControlPanel;

		private FileChangeNotify m_kFcn;

		private class PluginLoadException : ApplicationException
		{
			public PluginLoadException(string msg) : base (msg) {}
		}
		
		public MainForm()
		{
			//MessageBox.Show("block", "block", MessageBoxButtons.OKCancel);
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();
			/*
			m_kFcn = new FileChangeNotify("E:\\MyWorks\\1.���ι���\\����,����\\FileChangedNotify", "*.*", 
				NotifyFilters.LastWrite|NotifyFilters.CreationTime|NotifyFilters.LastAccess|NotifyFilters.FileName, 
				true);
			m_kFcn.ChangedHandler = null;
			m_kFcn.Start();
			*/
            CultureInfo newCInfo = (CultureInfo)Thread.CurrentThread.CurrentCulture.Clone();
            newCInfo.NumberFormat.NumberDecimalSeparator = ".";
            Thread.CurrentThread.CurrentCulture = newCInfo;
			WinFormsUtility.MainForm = this;
			WinFormsUtility.DockPanel = this.m_ctlDockPanel;
			LoadPlugins();
			CreatePanels();

			InitializeGUI();

			ServiceProvider sp = ServiceProvider.Instance;
			m_uiCommandService = sp.GetService(typeof(IUICommandService))
				as IUICommandService;

			MFramework.Instance.Startup();
		}

		private void CreatePanels()
		{
			ICommandPanelService pmCommandPanelService =
				ServiceProvider.Instance.GetService(typeof(ICommandPanelService)) as
				ICommandPanelService;

			m_kTriggerPanel = new TriggerPanel();
			pmCommandPanelService.RegisterPanel(m_kTriggerPanel);
			m_kTriggerPanel.SetXmlManager(ref m_kXmlManager);

			m_kMonsterPanel = new MonsterPanel();
			pmCommandPanelService.RegisterPanel(m_kMonsterPanel);

			m_kPathPanel = new PathPanel();
			pmCommandPanelService.RegisterPanel(m_kPathPanel);

			m_kNpcPanel = new NpcPanel();
			pmCommandPanelService.RegisterPanel(m_kNpcPanel);
			m_kNpcPanel.SetXmlManager(ref m_kXmlManager);

			m_kMonsterAreaPanel = new MonsterArea();
			pmCommandPanelService.RegisterPanel(m_kMonsterAreaPanel);

			m_kWayPointPanel = new WayPointPanel();
			pmCommandPanelService.RegisterPanel(m_kWayPointPanel);

			m_kXmlPropertiesPanel = new XmlPropertiesPanel();
			pmCommandPanelService.RegisterPanel(m_kXmlPropertiesPanel);
			m_kXmlPropertiesPanel.SetXmlManager(ref m_kXmlManager);

			m_kNotExistFilesPanel = new NotExistFilesPanel();
			pmCommandPanelService.RegisterPanel(m_kNotExistFilesPanel);

			m_kStonePanel = new StonePanel();
			pmCommandPanelService.RegisterPanel(m_kStonePanel);
			m_kStonePanel.SetXmlManager(ref m_kXmlManager);

			m_kFogPanel = new FogPanel();
			pmCommandPanelService.RegisterPanel(m_kFogPanel);
			m_kFogPanel.SetXmlManager(ref m_kXmlManager);

			m_kZoneControlPanel = new ZoneControl();
			pmCommandPanelService.RegisterPanel(m_kZoneControlPanel);
			
            //m_kDecalPanel = new DecalPanel();
            //pmCommandPanelService.RegisterPanel(m_kDecalPanel);

            m_kBreakObjectPanel = new BreakObjectPanel();
            pmCommandPanelService.RegisterPanel(m_kBreakObjectPanel);
            m_kBreakObjectPanel.SetXmlManager(ref m_kXmlManager);

            m_kElevatorPanel = new ElevatorPanel();
            pmCommandPanelService.RegisterPanel(m_kElevatorPanel);
            m_kElevatorPanel.SetXmlManager(ref m_kXmlManager);
		}

		private ShortcutServiceImpl ShortcutService
		{
			get
			{
				if (m_shortcutService == null)
				{
					ServiceProvider sp = ServiceProvider.Instance;
					m_shortcutService = 
						sp.GetService(typeof(IShortcutService)) 
						as ShortcutServiceImpl;					
				}
				return m_shortcutService;
			}
		}

		private void TimerCallBack(object sender, EventArgs e)
		{
			try
			{
				//It may be possible to get this callback in the middle of
				//Shutting things down, so guard against that
				if (!this.Disposing && !this.IsDisposed)
				{
					m_idleCommand.DoClick(this, null);
				}
			}
			catch (Exception ex)
			{
				Debug.WriteLine("Exception thrown in timer code:");
				Debug.WriteLine(ex);
			}
		}

		private void LoadPlugins()
		{
			try 
			{
				string pluginPath = 
					MFramework.Instance.AppStartupPath + @"plugins";

				PluginManager.LoadPlugins(pluginPath);
			}
			catch (Exception e)
			{
				//This amounts to a catastrophic error. The application
				//cannot recover from this, so we will print this dialog
				//and throw a PlinLoadException, which will cause the app 
				//to exit
				Console.WriteLine("Uncought exception loading plugins:");
				Console.WriteLine(e);
				MessageBox.Show("Application could not load plugins" + 
					Environment.NewLine +
					e.ToString(),
								"Critical Error");
				throw new PluginLoadException(
					   "Plugins could not load properly");
			}
		}

		private static string ms_strDisplayNewSceneFormSettingName =
			"DisplayNewSceneForm";
		private static string ms_strDisplayNewSceneFormOptionName =
			"Palettes.Prompt to Select Palette Folder for New Scenes";

		private void InitializeGUI()
		{
			ServiceProvider sp = ServiceProvider.Instance;
			IUICommandService commandService =
				sp.GetService(typeof(IUICommandService))
				as IUICommandService;
	
			commandService.BindCommands(this);
	
	
			ISettingsService settingsService = 
				sp.GetService(typeof(ISettingsService))
				as ISettingsService;

			settingsService.RegisterSettingsObject(
				"AutoSaveIntervalInMinutes", 1, SettingsCategory.PerUser);

			settingsService.RegisterSettingsObject(
				ms_strDisplayNewSceneFormSettingName, true,
				SettingsCategory.PerUser);

			IOptionsService pmOptionsService = sp.GetService(
				typeof(IOptionsService)) as IOptionsService;
			Debug.Assert(pmOptionsService != null,
				"Options service not found!");

			pmOptionsService.AddOption(ms_strDisplayNewSceneFormOptionName,
				SettingsCategory.PerUser,
				ms_strDisplayNewSceneFormSettingName);
			pmOptionsService.SetHelpDescription(
				ms_strDisplayNewSceneFormOptionName, "Indicates whether or " +
				"not a dialog box is displayed when a new scene is created " +
				"that prompts the user to select a palette folder " +
				"location. If this option is set to false, the most " +
				"recently used palette folder location will be used for " +
				"the new scene.");
	
			MenuServiceImpl menuService =
				sp.GetService(typeof(IMenuService)) as MenuServiceImpl;



			LoadMainWindowPosition();

			menuService.AnnotateMenus();
			ComponentManagementUtilities.BuildComponentListFromPalettes(
				MFramework.Instance.PaletteManager.GetPalettes());
			MRUManager.Init();
		}

		private void LoadMainWindowPosition()
		{
			ServiceProvider sp = ServiceProvider.Instance;
			ISettingsService settingsService = 
				sp.GetService(typeof(ISettingsService))
				as ISettingsService;

			object objWindowState = settingsService.GetSettingsObject(
				"MainWindowState", SettingsCategory.PerUser);

			object objWindowSize = settingsService.GetSettingsObject(
				"MainWindowSize", SettingsCategory.PerUser);

			object objWindowLocation = settingsService.GetSettingsObject(
				"MainWindowLocation", SettingsCategory.PerUser);

			if (objWindowState != null)
			{
				if (objWindowSize != null && objWindowLocation != null)
				{
					this.Size = (Size)objWindowSize;
					this.Location = (Point)objWindowLocation;
				}

				FormWindowState ws = (FormWindowState)objWindowState;
				if (ws != FormWindowState.Minimized)
				{
					this.WindowState = ws;
				}
			}
			else
			{
				this.WindowState = FormWindowState.Maximized;
			}
		}

		private void SaveMainWindowPosition()
		{
			if (this.WindowState != FormWindowState.Minimized)
			{
				ServiceProvider sp = ServiceProvider.Instance;
				ISettingsService settingsService =
					sp.GetService(typeof(ISettingsService))
					as ISettingsService;

				settingsService.SetSettingsObject("MainWindowState",
					this.WindowState, SettingsCategory.PerUser);

				if (this.WindowState != FormWindowState.Maximized)
				{
					settingsService.SetSettingsObject("MainWindowSize",
						this.Size, SettingsCategory.PerUser);

					settingsService.SetSettingsObject("MainWindowLocation",
						this.Location, SettingsCategory.PerUser);
				}
			}
		}

		[UICommandHandler("ReCheckSceneAfterPaletteImport")]
		private void OnReCheckScene(object sender, EventArgs args)
		{
			MPalette[] palettes = 
				MFramework.Instance.PaletteManager.GetPalettes();
			MScene scene =MFramework.Instance.Scene;

			ConflictManagementUtilities.CheckForConflicts(scene,
				palettes);
			MPalette orphans = 
				EntityManagementUtilities.ResolveMasterEntities(
				scene, palettes);
			//We should never get orphans as the result of importing a palette
			Debug.Assert(orphans.Scene.GetEntities().Length == 0);
		}

		[UICommandHandler("SavePalettes")]
		private void OnSavePalettes(object sender, EventArgs args)
		{
			//SavePalettes();
		}

		private void SavePalettes()
		{
			MPalette[] dirtyPalettes = GetDirtyPalettes();

			if (dirtyPalettes.Length == 0)
			{
				return;
			}

			//if (!CheckPaletteFolderExists())
			//{
			//	return;
			//}

			//if (!CheckForReadOnlyPaletteFiles(dirtyPalettes))
			//{
			//	return;
			//}

			string[] streamingExtensions =
				MUtility.GetStreamingFormatExtensions();

			MPaletteManager paletteManager =
				MFramework.Instance.PaletteManager;

			//string paletteFolder = paletteManager.PaletteFolder;

			foreach (MPalette palette in paletteManager.GetPalettes())
			{
				if (palette.DontSave)
				{
					if (palette.Scene.Dirty)
					{
						DirtyBitUtilities.MakeSceneClean(palette.Scene);
					}
					continue;
				}

				if (!palette.Scene.Dirty)
					continue;

				SceneManagementUtilities.SetNameOnTemplates(palette);

				string paletteFileName = palette.Path + palette.Name
					+ "." + m_paletteFileExtension;

				FileInfo fi = new FileInfo(paletteFileName);
				bool bReadonly = (fi.Attributes & FileAttributes.ReadOnly)
					== FileAttributes.ReadOnly;

				if (fi.Exists && bReadonly)
				{
					fi.Attributes = fi.Attributes &
						~FileAttributes.ReadOnly;
				}

				string strPath = palette.Path;

				paletteManager.SavePalette(palette,
					paletteFileName, streamingExtensions[0]); 
				
				DirtyBitUtilities.MakeSceneClean(palette.Scene);
			}
			
		}

		/// <summary>
		/// Checks to make sure the palette files are writeable
		/// </summary>
		/// <param name="palettes"></param>
		/// <returns>true</returns>
		private bool CheckForReadOnlyPaletteFiles(MPalette[] palettes)
		{
			MPaletteManager paletteManager =
				MFramework.Instance.PaletteManager;
			string folder = paletteManager.PaletteFolder;

			ArrayList readonlyPaletteFiles = new ArrayList();

			foreach (MPalette palette in palettes)
			{
				string palettePath = string.Format("{0}{1}.{2}",
					folder, palette.Name, m_paletteFileExtension);
				FileInfo fi = new FileInfo(palettePath);
				if (fi.Exists)
				{
					if ((fi.Attributes & FileAttributes.ReadOnly) == 
						FileAttributes.ReadOnly)
					{
						readonlyPaletteFiles.Add(fi.FullName);
					}
				}
			}
			if (readonlyPaletteFiles.Count > 0)
			{
				StringBuilder sb = new StringBuilder();
				sb.Append("The following palette file(s) are read-only.\n");
				foreach (string paletteFile in readonlyPaletteFiles)
				{
					sb.AppendFormat("{0}\n", paletteFile);
				}
				sb.Append("Would you like to over write them?\n");
				sb.Append(
					"Note: If you click 'No', none of your palettes will be saved");
				string messageText = sb.ToString();
				DialogResult result = MessageBox.Show(messageText,
					"Palette Files Are Read Only.", 
					MessageBoxButtons.YesNo, MessageBoxIcon.Question);
				return (result == DialogResult.Yes);
			}
			return true;
		}


		private MPalette[] GetDirtyPalettes()
		{
			MPaletteManager paletteManager = 
				MFramework.Instance.PaletteManager;
			ArrayList dirtyPalettes = new ArrayList();

			foreach (MPalette palette in paletteManager.GetPalettes())
			{
				if (palette.Scene.Dirty)
				{
					dirtyPalettes.Add(palette);
				}
			}
			return dirtyPalettes.ToArray(typeof(MPalette)) as MPalette[];
		}

		private bool CheckPaletteFolderExists()
		{
			MPaletteManager paletteManager = 
				MFramework.Instance.PaletteManager;
			DirectoryInfo di = new DirectoryInfo(paletteManager.PaletteFolder);
			if (!di.Exists)
			{
				DialogResult result = 
					MessageBox.Show("The Palette Folder:\n" + 
					paletteManager.PaletteFolder + "\n"+
					"does not exist. Would you like to create it?\n"
					+"Note: if you chose not to create the folder, \n" +
					"your palettes will not be saved.",
					"Palette Folder Not Found", MessageBoxButtons.YesNo,
					MessageBoxIcon.Question);
				if (result == System.Windows.Forms.DialogResult.Yes)
				{
					di.Create();
					return true;
				}
				else
				{
					return false;
				}
			}
			return true;
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.components = new System.ComponentModel.Container();
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MainForm));
			this.m_ctlDockPanel = new WeifenLuo.WinFormsUI.DockPanel();
			this.m_menuMain = new System.Windows.Forms.MainMenu(this.components);
			this.m_timerComponent = new System.Windows.Forms.Timer(this.components);
			this.SuspendLayout();
			// 
			// m_ctlDockPanel
			// 
			this.m_ctlDockPanel.ActiveAutoHideContent = null;
			this.m_ctlDockPanel.Dock = System.Windows.Forms.DockStyle.Fill;
			this.m_ctlDockPanel.Font = new System.Drawing.Font("Tahoma", 11F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.World);
			this.m_ctlDockPanel.Location = new System.Drawing.Point(0, 0);
			this.m_ctlDockPanel.Name = "m_ctlDockPanel";
			this.m_ctlDockPanel.Size = new System.Drawing.Size(1272, 990);
			this.m_ctlDockPanel.TabIndex = 0;
			this.m_ctlDockPanel.Paint += new System.Windows.Forms.PaintEventHandler(this.m_ctlDockPanel_Paint);
			// 
			// m_timerComponent
			// 
			this.m_timerComponent.Enabled = true;
			this.m_timerComponent.Interval = 1000;
			this.m_timerComponent.Tick += new System.EventHandler(this.m_timerComponent_Tick);
			// 
			// MainForm
			// 
			this.AllowDrop = true;
			this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
			this.ClientSize = new System.Drawing.Size(1272, 990);
			this.Controls.Add(this.m_ctlDockPanel);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.IsMdiContainer = true;
			this.Menu = this.m_menuMain;
			this.Name = "MainForm";
			this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
			this.Text = "Gamebryo Scene Designer";
			this.DragDrop += new System.Windows.Forms.DragEventHandler(this.OnDragDrop);
			this.Closing += new System.ComponentModel.CancelEventHandler(this.MainForm_Closing);
			this.DragEnter += new System.Windows.Forms.DragEventHandler(this.OnDragEnter);
			this.Load += new System.EventHandler(this.MainForm_Load);
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main()
		{
			m_restartLock = new Mutex(false, "SceneDesignerRestart");
			DateTime before = DateTime.Now;
			m_restartLock.WaitOne();
			DateTime after = DateTime.Now;
			Debug.WriteLine(
			"Waited " + (after - before).TotalMilliseconds.ToString()
				+ "ms for startup mutext");
			m_restartLock.ReleaseMutex();		   
			m_restartLock = null;

			//Now, allow only one instance.
			string mutexGuid = "{A8DE0E56-4130-4f92-A3D4-391D7A799F96}";
			// {A8DE0E56-4130-4f92-A3D4-391D7A799F96}

			m_instanceLock = new Mutex(false, mutexGuid);

			bool aquiredLock = m_instanceLock.WaitOne(new TimeSpan(0, 0, 1),
				true);

			/*
			if (!aquiredLock)
			{
				string message = string.Format(
					"The {0} is already running.\n" 
					+ "Only one instance is supported",
					Application.ProductName);
					MessageBox.Show(message, 
						string.Format("{0}: cannot launch",
						Application.ProductName), 
					MessageBoxButtons.OK, MessageBoxIcon.Error);
				return;
			}
			*/

			MFramework.Init();

			Invalidator.Init();

			try
			{
				using (new EnableThemingInScope(true))
				{
					MainForm mainForm = new MainForm();

					// Register idle handler.
					Application.Idle += new EventHandler(mainForm.OnIdle);

					Application.Run(mainForm);
				}
			}
			catch (PluginLoadException)
			{}
			finally
			{
				//MFramework.Instance.Scene.RemoveAllEntities(false);
				ServiceProvider.Instance.Shutdown();
				PluginManager.Shutdown();
				Invalidator.Shutdown();
				MFramework.Shutdown();
				ShutdownAssemblies();
			}

			m_instanceLock.ReleaseMutex();

			if (m_restartLock != null)
			{
				m_restartLock.ReleaseMutex();
			}
		}

		private static void ShutdownAssemblies()
		{
			Assembly[] assemblies = AppDomain.CurrentDomain.GetAssemblies();
			foreach (Assembly assembly in assemblies)
			{
				AssemblyUtilities.FindAndCallShutdownMethod(assembly);
			}
		}

		private bool IdleProcess()
		{
			// ExtEntity
			if (MFramework.Instance.ExtEntity.m_bHaveWaitAddEntity)
			{
				Process_AddExtEntity();
			}
			if (MFramework.Instance.ExtEntity.m_bHaveWaitDeleteEntity)
			{
				Process_DeleteExtEntity();
			}
			if (MFramework.Instance.ExtEntity.m_bHaveWaitTranslateEntity)
			{
				Process_TranslateExtEntity();
			}
			if (MFramework.Instance.ExtEntity.m_bHaveWaitSelectEntity)
			{
				Process_SelectExtEntity();
			}

			return true;
		}

		private void Process_AddExtEntity()
		{
			ArrayList pmList = MFramework.Instance.ExtEntity.m_pmWaitAddEntities;
			MEntity pkEntity = (MEntity)pmList[0];
			if (pkEntity == null) return;
			pmList.Remove(pkEntity);
			MPgExtEntity.eExtEntityType eType = MFramework.Instance.ExtEntity.m_eWaitType;

			//
			if (eType == MPgExtEntity.eExtEntityType.ExtEntityType_ShineStone)
			{
				// ShineStone
				m_kStonePanel.AfterCreate(pkEntity);
			}

			if (pmList.Count <= 0)
			{
				MFramework.Instance.ExtEntity.m_bHaveWaitAddEntity = false;
			}
		}

		private void Process_DeleteExtEntity()
		{
			ArrayList pmList = MFramework.Instance.ExtEntity.m_pmWaitDeleteEntities;
			MEntity pkEntity = (MEntity)pmList[0];
			if (pkEntity == null) return;
			pmList.Remove(pkEntity);
			MPgExtEntity.eExtEntityType eType = MFramework.Instance.ExtEntity.m_eWaitType;

			//
			if (eType == MPgExtEntity.eExtEntityType.ExtEntityType_ShineStone)
			{
				// ShineStone
				m_kStonePanel.AfterDelete(pkEntity);
			}

			if (pmList.Count <= 0)
			{
				MFramework.Instance.ExtEntity.m_bHaveWaitDeleteEntity = false;
			}
		}

		private void Process_TranslateExtEntity()
		{
			ArrayList pmList = MFramework.Instance.ExtEntity.m_pmWaitTranslateEntities;
			MEntity pkEntity = (MEntity)pmList[0];
			if (pkEntity == null) return;
			pmList.Remove(pkEntity);
			MPgExtEntity.eExtEntityType eType = MFramework.Instance.ExtEntity.m_eWaitType;

			//
			if (eType == MPgExtEntity.eExtEntityType.ExtEntityType_ShineStone)
			{
				// ShineStone
				m_kStonePanel.AfterTranslate(pkEntity);
			}

			if (pmList.Count <= 0)
			{
				MFramework.Instance.ExtEntity.m_bHaveWaitTranslateEntity = false;
			}
		}

		private void Process_SelectExtEntity()
		{
			ArrayList pmList = MFramework.Instance.ExtEntity.m_pmWaitSelectEntities;
			MEntity pkEntity = (MEntity)pmList[0];
			if (pkEntity == null) return;
			pmList.Remove(pkEntity);
			MPgExtEntity.eExtEntityType eType = MFramework.Instance.ExtEntity.m_eWaitType;

			//
			if (eType == MPgExtEntity.eExtEntityType.ExtEntityType_ShineStone)
			{
				// ShineStone
				m_kStonePanel.AfterSelect(pkEntity);
			}

			if (pmList.Count <= 0)
			{
				MFramework.Instance.ExtEntity.m_bHaveWaitSelectEntity = false;
			}
		}

		private void OnIdle(object sender, EventArgs e)
		{
			if (this.Visible &&
				this.WindowState !=  FormWindowState.Minimized &&
				Form.ActiveForm == this)
			{
				IdleProcess();

				Invalidator.Instance.Update();
			}
		}

		private string BuildTitleBarText(string fileName)
		{
			string title = string.Format("{0} - {1}", fileName, 
				MFramework.Instance.ApplicationName);
			if (MFramework.Instance.Scene.Dirty)
			{
				title = title + "*";
			}
			return title;
		}

		private WeifenLuo.WinFormsUI.DockPanel m_ctlDockPanel;
		private System.Windows.Forms.MainMenu m_menuMain;

		private void MainForm_Load(object sender, System.EventArgs e)
		{
			ServiceProvider sp = ServiceProvider.Instance;

			IMessageService messageService = 
				sp.GetService(typeof(IMessageService)) as IMessageService;
			
			ISettingsService settingsService = 
				sp.GetService(typeof(ISettingsService))
				as ISettingsService;

			CommandPanelServiceImpl panelService =
				sp.GetService(typeof(ICommandPanelService))
				as CommandPanelServiceImpl;


			string dockConfig = settingsService.GetSettingsObject(
				dockConfigSettingName, SettingsCategory.PerUser) as string;

			bool panelLayoutLoaded = false;
			try
			{
				if (dockConfig != null)
				{
					using (Stream s = new MemoryStream(
							   ASCIIEncoding.UTF8.GetBytes(dockConfig)))
					{						
						panelService.LoadConfig(s);
						panelLayoutLoaded = true;
					}
				}
			}
			catch(Exception)
			{
				messageService.AddMessage(MessageChannelType.General, 
					"Panel Layout not found or couldn't load.\n" +
					"Default layout will be loaded");
				
			}
			if (!panelLayoutLoaded)
			{
				try
				{
					using (Stream resourceStream = 
							   Assembly.GetExecutingAssembly()
							   .GetManifestResourceStream(
							   "Emergent.Gamebryo.SceneDesigner.GUI" +
							   ".DefaultDocking.config"))
					{
						panelService.LoadConfig(resourceStream);
					}
				}	
				catch (InvalidOperationException)
				{
					//Something was not right about the format...
					messageService.AddMessage(MessageChannelType.General, 
						"Panel Layout Failed to load.");

				}
			}

			AssociateFileTypes.Associate(".gsa", 
				Application.ExecutablePath, "GSA.Document",
				"Gamebryo Scene Ascii File", Application.StartupPath +
				@"\..\..\Data\GSADoc.ico");


			RenderForm renderForm = 
				panelService.GetPanel("Viewports") as RenderForm;

			Invalidator.Instance.RegisterControl(renderForm);
			MakeNewFile(false);
			m_idleCommand = m_uiCommandService.GetCommand("Idle");
			m_timer = new TimedInvoker();
			m_timer.Interval = new TimeSpan(0, 0, 0, 0, 100);
			m_timer.SychronizedObject = this;
			m_timer.CallBack = new EventHandler(TimerCallBack);
			m_timer.Start();
			if (!CheckForAutosaveFiles())
			{
				string[] args = Environment.GetCommandLineArgs();
				if (args.Length > 1)
				{
					string fileName = args[1];
					StringBuilder longPath = new StringBuilder(
						NativeWin32.MAX_PATH);
					NativeWin32.GetLongPathName(fileName, longPath,
						NativeWin32.MAX_PATH);
					string longFileName = longPath.ToString();
					FileInfo fi = new FileInfo(longFileName);
					
					string extension = fi.Extension.Substring(1).ToUpper();

					LoadMainScene(longFileName, extension, false);
				}
			}

			string strURL = Environment.GetEnvironmentVariable("NDL_PATH");
			if (strURL != null)
				strURL = strURL + @"\Documentation\HTML\Gamebryo.chm";

			string strBaseSceneDesignerHelpPath = 
				@"/Tool_Manuals/Scene_Designer/";
			HelpManager.Instance.Init(strURL, strBaseSceneDesignerHelpPath);

			MFramework.Instance.EventManager.LongOperationStarted +=
				new MEventManager.__Delegate_LongOperationStarted(
				this.OnLongOperationStarted);
			MFramework.Instance.EventManager.LongOperationCompleted +=
				new MEventManager.__Delegate_LongOperationCompleted(
				this.OnLongOperationCompleted);

			//Note jwolfe 3/13/06
			//This seems to be necessary to make the main form's window
			//Show up in the task bar.
			//With out this call, the user must alt-tab back to the window
			//to get it to show up...

			NativeWin32.SetActiveWindow((IntPtr) null);
			NativeWin32.SetActiveWindow(this.Handle);
		}

		private void OnLongOperationStarted()
		{
			this.Cursor = Cursors.WaitCursor;
		}

		private void OnLongOperationCompleted()
		{
			this.Cursor = Cursors.Default;
		}

		private void OnDragEnter(object sender, 
			System.Windows.Forms.DragEventArgs e)
		{
			Debug.WriteLine(new StackTrace().GetFrame(0).ToString());
			if (e.Data.GetDataPresent(DataFormats.FileDrop, false))
			{
				e.Effect = DragDropEffects.All;
			}
		}

		private void OnDragDrop(object sender, 
			System.Windows.Forms.DragEventArgs e)
		{
			string[] filenames = e.Data.GetData(DataFormats.FileDrop) 
				as string[];
			if (filenames.Length > 1)
			{
				MessageBox.Show("Only one Scene file may be opened at a time."
					,"Can not open multiple files", MessageBoxButtons.OK, 
					MessageBoxIcon.Error);
			}
			else
			{
				if (!CheckBeforeLosingChanges())
				{				
					return;
				}				
				FileInfo fi = new FileInfo(filenames[0]);
				string extension = fi.Extension.Substring(1).ToUpper();
				LoadMainScene(filenames[0], extension, false);
			}
		}


		[UICommandHandler("Exit")]
		private void m_miExit_Click(object sender, System.EventArgs e)
		{
			this.Close();
		}

		private void SaveWindowFormToSettingfile()
		{
			ServiceProvider sp = ServiceProvider.Instance;
			ISettingsService settingsService =
				sp.GetService(typeof(ISettingsService))
				as ISettingsService;
			CommandPanelServiceImpl panelService =
				sp.GetService(typeof(ICommandPanelService))
				as CommandPanelServiceImpl;

			if (!m_bClearWindowLayout)
			{
				using (MemoryStream ms = new MemoryStream())
				{
					panelService.SaveConfig(ms);
					Encoding.UTF8.GetString(ms.GetBuffer());
					string dockConfig =
						Encoding.UTF8.GetString(ms.GetBuffer());
					settingsService.SetSettingsObject(dockConfigSettingName,
						dockConfig, SettingsCategory.PerUser);
				}
			}
			else
			{
				settingsService.RemoveSettingsObject(dockConfigSettingName,
					SettingsCategory.PerUser);
			}

			SaveMainWindowPosition();

			settingsService.SaveSettings(SettingsCategory.Global);


			//������� ������ �켱���ϱ� ����
			//������� UI������ �ҷ����� �ʾ��� ��츸 ���� ������ �����Ѵ�
			if (!m_bCustomUILoaded)
			{
				settingsService.SaveSettings(SettingsCategory.PerUser);
			}
		}

		private void MainForm_Closing(object sender, 
			System.ComponentModel.CancelEventArgs e)
		{
			if (!CheckBeforeLosingChanges())
			{
				e.Cancel = true;
				return;
			}
				

			m_timer.Stop();

			SaveWindowFormToSettingfile();
			
			//SavePalettes();
			DeleteAutoSave();
		}


		/// <summary>
		/// Makes sure the user has saved the scene if it has changed
		/// </summary>
		/// <returns>return value of false means the caller should cancle
		/// whatever it was about to do.</returns>
		private bool CheckBeforeLosingChanges()
		{
			if (!MFramework.Instance.Scene.Dirty)
			{
				return true;
			}
			DialogResult result = MessageBox.Show(
				"The current file has been modified.\nWould you like to " +
				"save before continuing?", "Save Changed File?",
				MessageBoxButtons.YesNoCancel, MessageBoxIcon.Warning);
			switch (result)
			{
				case System.Windows.Forms.DialogResult.Yes:
					{
						OnSave(this, null);
						break;
					}
				case System.Windows.Forms.DialogResult.No:
					{
						return true;
					}
				case System.Windows.Forms.DialogResult.Cancel:
					{
						return false;
					}
			}
			return true;
		}

		#region UICommand Hanlders
		[UICommandHandler("SaveFile")]
		private void OnSave(object sender, EventArgs args)
		{
			if (m_currentFileName.Equals(string.Empty))
			{
				OnSaveFileAs(sender, args);
			}
			else
			{
				FileInfo fi = new FileInfo(m_currentFileName);
				SaveMainScene(m_currentFileName, 
					fi.Extension.Replace(".", "").ToUpper(), false);
			}
		}

		[UICommandHandler("SaveFileAs")]
		private void OnSaveFileAs(object sender, EventArgs args)
		{
			SaveFileDialog dlg = new SaveFileDialog();

			string[] streamingDescriptions = 
				MUtility.GetStreamingFormatDescriptions();
			string[] streamingExtensions =
				MUtility.GetStreamingFormatExtensions();

			int iFormats = streamingDescriptions.Length;
			Debug.Assert(iFormats > 0);
			dlg.Filter = streamingDescriptions[0];
			for (int i=1; i<iFormats; i++)
			{
				dlg.Filter = dlg.Filter + "|" + streamingDescriptions[i];
			}
			if (dlg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
			{
				string fileName = dlg.FileName;
				string extension = streamingExtensions[dlg.FilterIndex-1];
				SaveMainScene(fileName, extension, false);
			}
		}

		//
		// GSA������ ������ �� ���� ����� .settings������ �������� ������ �ϳ���
		// ����� �ϱ� ���� �۾�
		// �۵����� : Save��ư�� ������ SaveMainScene�Լ��� �� �ι� ����
		// ù��° ���� : �ش� ������ �ִ� ������ GSA������ ���� �� �� ������
		//               �������̸�.Scene.setting ������ �����.
		// �ι�° ���� : �ڵ�����, �ڵ����������� �����ϰ� 
		//               �ڵ����������̸�.Scene.settings ������ �����. 
		//
		private void SaveMainScene(string fileName, string extension,
			bool bAutosave)
		{
			MFramework fw = MFramework.Instance;
			FileInfo fi = new FileInfo(fileName);
			if (fi.Exists)
			{
				bool bReadonly = (fi.Attributes & FileAttributes.ReadOnly)
					== FileAttributes.ReadOnly;
				if (bReadonly)
				{
					bool bOverwrite = 
						MessageBox.Show(fi.FullName + Environment.NewLine +
						"	is read only. Would you like to over write it?",
						"Can not save file", MessageBoxButtons.YesNo)
						== System.Windows.Forms.DialogResult.Yes;
					if (!bOverwrite)
						return;
					fi.Attributes = fi.Attributes & (~FileAttributes.ReadOnly);
				}
			}
			SceneManagementUtilities.PutSceneItemsIntoSettings(fw.Scene);
			// �ȷ�Ʈ ����.

			if (bAutosave)
			{
				fw.SavePalettesDir(extension, null, bAutosave);
			}
			else
			{
				fw.SavePalettesDir(extension, fi.DirectoryName, bAutosave);
			}

			bool fileSaved = fw.SaveScene(fileName, extension, bAutosave);
			if (!fileSaved)
			{
				MessageBox.Show("Failed to save scene");
			}
			else
			{
				
				ServiceProvider sp = ServiceProvider.Instance;
				ISettingsService settingsService = 
					sp.GetService(typeof(ISettingsService))
						as ISettingsService;
				
				//�ڵ������� �ƴ� ��� Settings������ ���������� �ִ� ��ο� �ϳ��� ����
				if (false == bAutosave)
				{
					settingsService.ScenePath = Application.StartupPath;
				}
				else
				{
					settingsService.ScenePath = fi.DirectoryName;
				}
				settingsService.SceneFileName = "Latest.scene.settings";
				SceneManagementUtilities.ConvertScenePalettePathToRelative(
					fi.DirectoryName);
				settingsService.SaveSettings(SettingsCategory.PerScene); 
				SceneManagementUtilities.ConvertScenePalettePathToAbsolute(
					fi.DirectoryName);

				SavePalettes();
				if (!bAutosave)
				{
					m_currentFileName = fileName;
					DirtyBitUtilities.MakeSceneClean(
						MFramework.Instance.Scene);
					this.Text = BuildTitleBarText(fi.Name);
					MRUManager.AddFile(fileName);
				}
										  
			}
		}

		[UICommandHandler("XMLSaveFile")]
		private void OnXMLSaveFile(object sender, EventArgs args)
		{
			XmlDocument pDoc = m_kXmlManager.GetDocument();
			if (pDoc == null) return;

			if (m_kXmlManager.GetFilename() == null ||
				m_kXmlManager.GetFilename().Equals(string.Empty))
			{
				OnXMLSaveFileAs(sender, args);
			}
			else
			{
				FileInfo fi = new FileInfo(m_kXmlManager.GetFilename());
				SaveXMLFile(m_kXmlManager.GetFilename(),
					fi.Extension.Replace(".", "").ToUpper(), false);
			}
		}

		[UICommandHandler("XMLSaveFileAs")]
		private void OnXMLSaveFileAs(object sender, EventArgs args)
		{
			XmlDocument pDoc = m_kXmlManager.GetDocument();
			if (pDoc == null) return; SaveFileDialog dlg = new SaveFileDialog();

			string streamingDescriptions = "XML files (*.xml)|*.xml";
			string streamingExtensions = "XML";

			dlg.Filter = streamingDescriptions;
			if (dlg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
			{
				string fileName = dlg.FileName;
				string extension = streamingExtensions;
				SaveXMLFile(fileName, extension, false);
			}
		}

		private void SaveXMLFile(string fileName, string extension,
			bool bAutosave)
		{
			FileInfo fi = new FileInfo(fileName);
			if (fi.Exists)
			{
				bool bReadonly = (fi.Attributes & FileAttributes.ReadOnly)
					== FileAttributes.ReadOnly;
				if (bReadonly)
				{
					bool bOverwrite =
						MessageBox.Show(fi.FullName + Environment.NewLine +
						"	is read only. Would you like to over write it?",
						"Can not save file", MessageBoxButtons.YesNo)
						== System.Windows.Forms.DialogResult.Yes;
					if (!bOverwrite)
						return;
					fi.Attributes = fi.Attributes & (~FileAttributes.ReadOnly);
				}
			}

			// Other
			m_kStonePanel.SaveXML();

			//Fog����
			m_kFogPanel.SaveXML();

			//�̵�������Ʈ
			m_kElevatorPanel.RearrangeList(); //�̵�������Ʈ���� ����Ʈ�� ������ ���� ������Ʈ �׷���� ����
			m_kElevatorPanel.AlertAbnormalList(); //����Ʈ�� �ϳ��� ���� ������Ʈ �׷��� GUID�� ���

			
			// ����
			XmlDocument pDoc = m_kXmlManager.GetDocument();
			XmlTextWriter tr = new XmlTextWriter(fileName, Encoding.GetEncoding(949));
			tr.Formatting = Formatting.Indented;
			pDoc.WriteContentTo(tr);
			tr.Close();
		}

		[UICommandHandler("MRULoad")]
		private void OnMRULoad(object sender, EventArgs args)
		{
			if (!CheckBeforeLosingChanges())
			{				
				return;
			}
			UICommand mruSender = sender as UICommand;
			string fileName = mruSender.Data as string;
			FileInfo fi = new FileInfo(fileName);
			string extension = fi.Extension.Substring(1).ToUpper();
			LoadMainScene(fileName, extension, false);
		}

		[UICommandHandler("OpenFile")]
		private void OnOpenFile(object sender, EventArgs args)
		{
			if (!CheckBeforeLosingChanges())
			{				
				return;
			}
			OpenFileDialog dlg = new OpenFileDialog();

			string[] streamingDescriptions = 
				MUtility.GetStreamingFormatDescriptions();
			string[] streamingExtensions =
				MUtility.GetStreamingFormatExtensions();

			int iFormats = streamingDescriptions.Length;
			Debug.Assert(iFormats > 0);
			dlg.Filter = streamingDescriptions[0];
			for (int i=1; i<iFormats; i++)
			{
				dlg.Filter = dlg.Filter + "|" + streamingDescriptions[i];
			}

			//dlg.Filter = "Gamebryo ASCII Scene files (*.gsa)|*.gsa|" +
			//	"Gamebryo Binary Scene files (*.gsb)|*.gsb";
			if (dlg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
			{
				MFramework.Instance.Path.Clear();
				MFramework.Instance.PaletteManager.RemoveAllPalettes();
				MFramework.Instance.PaletteManager.ActivePalette = null;
				MFramework.Instance.PaletteManager.Refresh = true;

				IMessageService messageService = 
					ServiceProvider.Instance.GetService(
					typeof(IMessageService)) as IMessageService;
				messageService.ClearMessages(MessageChannelType.Conflicts);
				string sceneFilename = dlg.FileName;
				string extension = streamingExtensions[dlg.FilterIndex-1];
				MFramework.Instance.RemoveAllPaletteInDll(extension);
				LoadMainScene(sceneFilename, extension, false);
			}
		}

		private void LoadMainScene(string sceneFilename, string extension,
			bool bRecoveryFile)
		{
			MScene pmScene;

			//// Scene ���� �޸�
			//pmScene = MFramework.Instance.NewScene(0);
			//MFramework.Instance.InitNewScene(pmScene);
			
			// Ext Entity Init
			MFramework.Instance.ExtEntity.Clear();

			// '�������� �ʴ� ���� ����Ʈ' �� Ŭ����.
			MFramework.Instance.ExistFile.ClearNotExistFile();

			MFramework fw = MFramework.Instance;
			fw.PaletteManager.RemoveAllPalettes();
			fw.PaletteManager.ActivePalette = null;
			fw.Scene.RemoveAllEntities(false);
			pmScene = fw.LoadScene(sceneFilename, extension);
			string strAvailable = LoadPalettes(sceneFilename, extension, bRecoveryFile);
			if (null == pmScene)
			{
				MessageBox.Show("Failed to load scene");
			}
			else if (null != strAvailable)
			{
				MessageBox.Show("Failed to load " + strAvailable + " palette");
			}
			else
			{
				fw.PaletteManager.Refresh = true;

				ServiceProvider sp = ServiceProvider.Instance;
				ISettingsService settingsService =
					sp.GetService(typeof(ISettingsService))
						as ISettingsService;
				FileInfo fi = new FileInfo(sceneFilename);
				settingsService.ScenePath = fi.DirectoryName;
				settingsService.SceneFileName = fi.Name +
					".scene.settings";
				SceneManagementUtilities.ClearSceneSettings();
				settingsService.LoadSettings(SettingsCategory.PerScene);

				fw.InitNewScene(pmScene);

				SceneManagementUtilities.PutSettingsIntoSceneItems(
					fw.Scene);

				SceneManagementUtilities.ConvertScenePalettePathToAbsolute(
					fi.DirectoryName);
				SceneManagementUtilities.CopySceneSettingsToUserSettings(
					);

				SceneManagementUtilities.ResetPalettes();

				MPalette oldOrphan = fw.PaletteManager.GetPaletteByName(
					"Unsaved Scene Templates");
				if (null != oldOrphan)
				{
					fw.PaletteManager.RemovePalette(oldOrphan);
				}
				bool bConflictsFound =
					SceneManagementUtilities.CheckMainSceneForConflicts();

				ComponentManagementUtilities.BuildComponentListFromScene(
					fw.Scene);
				MPalette orphans =
					EntityManagementUtilities.ResolveMasterEntities(
						fw.Scene, fw.PaletteManager.GetPalettes());
				orphans.Name = "Unsaved Scene Templates";
				orphans.DontSave = true;

				if (!bRecoveryFile)
				{
					if (!bConflictsFound)
						DirtyBitUtilities.MakeSceneClean(
						MFramework.Instance.Scene);
					this.Text = BuildTitleBarText(fi.Name);
					m_currentFileName = sceneFilename;
					MRUManager.AddFile(m_currentFileName);
				}
			}

			MFramework.Instance.PgUtil.Initialize();
			//���� �ٿ�� ũ�⸦ ����
			m_kFogPanel.SetWorldBound();
		}

		private string LoadPalettes(string sceneFilename, string extension,
			bool bRecoveryFile)
		{
			MFramework fw = MFramework.Instance;

			MPaletteManager paletteManager =
				MFramework.Instance.PaletteManager;

			paletteManager.RemoveAllPalettes();

			ArrayList retVal = new ArrayList();
			IMessageService messageService =
				ServiceProvider.Instance.GetService(
				typeof(IMessageService)) as IMessageService;

			ArrayList pPalName = fw.LoadPaletteName(sceneFilename, extension);
			ArrayList pPalDir = fw.LoadPalettesDirectory(pPalName, sceneFilename, extension);
			int iCutIdx = sceneFilename.LastIndexOf("\\");
			string Filepath = sceneFilename.Remove(iCutIdx, sceneFilename.Length - iCutIdx);

            pPalName.Insert(0, "Default");
            pPalDir.Insert(0, "\\Data\\");
			for (int i = 0; i < pPalName.Count; i++)
			{
				string strDir = pPalDir[i].ToString();
				string strPalPathNName;
				string strError = Filepath + "\\" + pPalDir[i].ToString() + pPalName[i].ToString() + ".pal";

				// ���� �ϵ� ��ũ�� �ٸ��ٸ� ������.
                if (0 == i)
                {//Default�� ����������ġ���� ã��
                    strPalPathNName = Application.StartupPath + strDir + pPalName[i].ToString() + ".pal";
                }
                else if (bRecoveryFile)
				{
					strPalPathNName =
						pPalDir[i].ToString() + "\\" + pPalName[i].ToString() + ".pal";
				}
				else if (Filepath.Length > 0 && strDir.Length > 0 && (!Filepath.Substring(0, 1).Equals(strDir.Substring(0, 1)) ) )
				{
					strPalPathNName =
						Filepath + "\\" + pPalDir[i].ToString() + pPalName[i].ToString() + ".pal";
				}
				else
				{
					strPalPathNName = strDir + pPalName[i].ToString() + ".pal";
				}

				MPalette palette =
					paletteManager.LoadPalette(strPalPathNName);

				if (palette != null)
				{
					palette.ResetEntityNames();
					DirtyBitUtilities.MakeSceneClean(palette.Scene);
					retVal.Add(palette);
				}
				else
				{
					Message message = new Message();
					message.m_strText = "Could not load Palette file";

					message.m_strDetails = strPalPathNName +
						" Could not be loaded.\n" +
						" Check that the file is valid";
					messageService.AddMessage(
						MessageChannelType.Errors, message);

					return strError;
				}
			}
			 
			RemovepertyTypes(retVal);
			CheckAndRegisterPalettesAndPropertyTypes(retVal);

			//CheckMainSceneForConflicts();
			MPalette orphans =
				EntityManagementUtilities.ResolveMasterEntities(
				MFramework.Instance.Scene,
				MFramework.Instance.PaletteManager.GetPalettes());
			orphans.Name = "Unsaved Scene Templates";
			orphans.DontSave = true;
			SceneManagementUtilities.AddOrphansToPalettes(orphans);

			return null;
		}

		private static void RemovepertyTypes(ArrayList list)
		{
			foreach (MPalette palette in list)
			{
				ComponentManagementUtilities.
					RemoveRegisteredcomponentsInScene(palette.Scene);
			}
		}

		private static void CheckAndRegisterPalettesAndPropertyTypes(
			ArrayList palettes)
		{
			MPaletteManager paletteManager =
				MFramework.Instance.PaletteManager;

			foreach (MPalette palette in palettes)
			{
				// �̹� ��� �Ǿ� �ִ� �ȷ�Ʈ���
				// �� �ȷ�Ʈ �����ʹ� �����ϴ� ����� ���� ����Ѵ�.
				foreach (MPalette checkpalette in paletteManager.GetPalettes())
				{
					if (checkpalette.Name.Equals(palette.Name))
						paletteManager.RemovePalette(checkpalette);
				}

				ComponentManagementUtilities.BuildComponentListFromScene(
					palette.Scene);
				paletteManager.AddPalette(palette);
			}

			// Now mark all palletes as clean
			foreach (MPalette palette in paletteManager.GetPalettes())
			{
				DirtyBitUtilities.MakeSceneClean(palette.Scene);
			}

		}

		[UICommandHandler("MakeRainData")]
		private void OnMakeRainData(object sender, EventArgs args)
		{
			RainDataDialog dlg = new RainDataDialog();
			if (dlg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
			{
				MFramework.Instance.MakeRainData.Filename = dlg.m_strFilename;
				MFramework.Instance.MakeRainData.DotValue = dlg.m_fDotValue;
				MFramework.Instance.MakeRainData.IgnoreList = dlg.m_pkIgnoreList;
				MFramework.Instance.MakeRainData.IsBinaryMode = dlg.m_bIsBinaryMode;
				MFramework.Instance.MakeRainData.MakeRainDataToFile();
				MessageBox.Show(dlg.m_strFilename + "�� ����� �����ϴ�.", "Complete To Create Rain Data");
			}
		}

		[UICommandHandler("HideSomeObject")]
		private void OnHideSomeObject(object sender, EventArgs args)
		{
			MFramework.Instance.HideSomeObject.HideNeedlessObject();
		}

		[UICommandHandler("MakeMonsterWayPoint")]
		private void OnMakeMonsterWayPoint(object sender, EventArgs args)
		{
			SaveFileDialog dlg = new SaveFileDialog();

			string streamingDescriptions = "Monster Way Point files (*.way)|*.way";
			string streamingExtensions = "WAY";

			dlg.Filter = streamingDescriptions;
			if (dlg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
			{
				string fileName = dlg.FileName;
				string extension = streamingExtensions;

				MFramework.Instance.WayPoint.MakeFile(fileName);
			}
		}

		[UICommandHandler("LoadMonsterWayPoint")]
		private void OnLakeMonsterWayPoint(object sender, EventArgs args)
		{
			OpenFileDialog dlg = new OpenFileDialog();

			string streamingDescriptions = "Monster Way Point files (*.way)|*.way";
			string streamingExtensions = "WAY";

			dlg.Filter = streamingDescriptions;
			if (dlg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
			{
				string fileName = dlg.FileName;
				string extension = streamingExtensions;

				MFramework.Instance.WayPoint.LoadFile(fileName);
			}
		}

		private static IInteractionModeService m_pmInteractionModeService;
		private static IInteractionModeService InteractionModeService
		{
			get
			{
				if (m_pmInteractionModeService == null)
				{
					m_pmInteractionModeService = ServiceProvider.Instance
						.GetService(typeof(IInteractionModeService)) as
						IInteractionModeService;
					Debug.Assert(m_pmInteractionModeService != null,
						"Interaction mode service not found!");
				}
				return m_pmInteractionModeService;
			}
		}

		// HandOver, ������ 2008.01.29
		// MonArea�� �Ⱦ��ϴ�.
		//[UICommandHandler("MonAreaSave")]
		//private void OnMonAreaSave(object sender, EventArgs args)
		//{
		//    SaveFileDialog dlg = new SaveFileDialog();

		//    string streamingDescriptions = "MonArea XML files (*.xml)|*.xml";
		//    string streamingExtensions = "XML";

		//    dlg.Filter = streamingDescriptions;
		//    if (dlg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
		//    {
		//        string fileName = dlg.FileName;
		//        string extension = streamingExtensions;
		//        MFramework.Instance.MonArea.SaveToXML(fileName);
		//    }
		//}

		//[UICommandHandler("MonAreaLoad")]
		//private void OnMonAreaLoad(object sender, EventArgs args)
		//{
		//    OpenFileDialog dlg = new OpenFileDialog();

		//    string[] streamingDescriptions =
		//        MUtility.GetStreamingFormatDescriptions();
		//    string[] streamingExtensions =
		//        MUtility.GetStreamingFormatExtensions();

		//    int iFormats = streamingDescriptions.Length;
		//    Debug.Assert(iFormats > 0);
		//    dlg.Filter = streamingDescriptions[0];
		//    for (int i = 1; i < iFormats; i++)
		//    {
		//        dlg.Filter = dlg.Filter + "|" + streamingDescriptions[i];
		//    }

		//    // �ε� Ȯ����.
		//    dlg.Filter = "MonArea XML files (*.xml)|*.xml";
		//    if (dlg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
		//    {
		//        string fileName = dlg.FileName;
		//        string extension = streamingExtensions[dlg.FilterIndex - 1];
		//        MFramework.Instance.MonArea.LoadFromXML(fileName);

		//    }
		//}

		[UICommandHandler("TestMenu")]
		private void OnTestMenu(object sender, EventArgs args)
		{
			InteractionModeService.GetInteractionModeByName("GamebryoSelection").MouseMove(-999997, -999997);
		}
		
		[UICommandHandler("ChangePick")]
		private void OnChangePick(object sender, EventArgs args)
		{
			bool bCullFlag = MFramework.Instance.PickUtility.GetObserveAppCullFlag();
			MFramework.Instance.PickUtility.SetObserveAppCullFlag(!bCullFlag);
		}

		[UICommandHandler("AddAniComponentToPalettes")]
		private void AddAniComponentToPalettes(object sender, EventArgs args)
		{
			MFramework.Instance.PgUtil.AddAniComponentToPalettes();
		}

		[UICommandHandler("SyncObjectToPalettes")]
		private void SyncObjectToPalettes(object sender, EventArgs args)
		{
			MFramework.Instance.PgUtil.SyncObjectToPalettes();
		}
						
		[UICommandHandler("ViewNotExistFiles")]
		private void OnViewNotExistFiles(object sender, EventArgs args)
		{
		}

		[UICommandHandler("XMLOpenFile")]
		private void OnXMLOpenFile(object sender, EventArgs args)
		{
			OpenFileDialog dlg = new OpenFileDialog();

			string[] streamingDescriptions =
				MUtility.GetStreamingFormatDescriptions();
			string[] streamingExtensions =
				MUtility.GetStreamingFormatExtensions();

			int iFormats = streamingDescriptions.Length;
			Debug.Assert(iFormats > 0);
			dlg.Filter = streamingDescriptions[0];
			for (int i = 1; i < iFormats; i++)
			{
				dlg.Filter = dlg.Filter + "|" + streamingDescriptions[i];
			}

			// �ε� Ȯ����.
			dlg.Filter = "XML files (*.xml)|*.xml";
			if (dlg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
			{
				string sceneFilename = dlg.FileName;
				string extension = streamingExtensions[dlg.FilterIndex - 1];
				m_kXmlManager.XmlFileLoad(sceneFilename);

				m_kTriggerPanel.PanelListLoad();
				m_kNpcPanel.InitPanelListLoad();
				m_kXmlPropertiesPanel.PanelListLoad();
				m_kStonePanel.PanelListLoad();
                m_kElevatorPanel.InitPanelListLoad();
				m_kFogPanel.LoadXML();
			}
		}

		[UICommandHandler("ImportFromGSA")]
		private void ImportFromGSA(object sender, EventArgs args)
		{
			OpenFileDialog kOpenfiledlg = new OpenFileDialog();
			kOpenfiledlg.Filter = "Gamebryo ASCII Scene file (*.gsa)|*.gsa";

			if (System.Windows.Forms.DialogResult.OK == kOpenfiledlg.ShowDialog())
			{
				MergePanel kMergePanel = new MergePanel(kOpenfiledlg.FileName);
				if (System.Windows.Forms.DialogResult.OK == kMergePanel.ShowDialog())
				{
					MessageBox.Show("�׽�Ʈ����");
				}
			}
		}

		// �� UI �⺻ �������� ��� �������� �Լ�
		private string GetUISettingfilePath()
		{
			string kPathStr= Environment.GetFolderPath(
					Environment.SpecialFolder.LocalApplicationData);
			string kGamebryoFolderStr = @"\Emergent Game Technologies\" +
				MFramework.Instance.ApplicationName + @"\";
			string kFileNameStr = "Tool.Settings";
			string kFullPathofSettingFileStr = kPathStr+ kGamebryoFolderStr + kFileNameStr;

			return kFullPathofSettingFileStr;
		}

		// �� UI ���� �����ϱ�
		[UICommandHandler("UISave")]
		private void OnUISave(object sender, EventArgs args)
		{
			//���� ���� �� ���� �����췹�̾ƿ� ����
			SaveWindowFormToSettingfile();
			
			SaveFileDialog kSavefiledlg = new SaveFileDialog();
			kSavefiledlg.Filter = "Settings files (*.settings)|*.settings";

			if (System.Windows.Forms.DialogResult.OK == kSavefiledlg.ShowDialog())
			{
				string kUserSettingFileStr = kSavefiledlg.FileName;
				//���� �� ���� ���� ���
				string kFullPathofSettingFileStr = GetUISettingfilePath();
				
				//���� ui�������� ��ü
				FileInfo kfiSettingFile = null;

				try
				{
					kfiSettingFile = new FileInfo(kFullPathofSettingFileStr);
					kfiSettingFile.OpenRead();
				}
				catch (FileNotFoundException)
				{
					MessageBox.Show(kFullPathofSettingFileStr + "\n ��ġ�� �⺻ ���� ������ �����ϴ�.\n"
						+ "�ŵ����̳ʸ� ���� �� �ٽ� �������ּ���.");
					return;
				}

				try
				{
					kfiSettingFile.CopyTo(kUserSettingFileStr, true);
				}
				catch
				{
					MessageBox.Show("���� ���� ����.");
				}
			}
		}
		
		// �� UI ���� �ҷ�����
		[UICommandHandler("UILoad")]
		private void OnULoad(object sender, EventArgs args)
		{
			OpenFileDialog kOpenfiledlg = new OpenFileDialog();
			kOpenfiledlg.Filter = "Settings files (*.settings)|*.settings";

			if (System.Windows.Forms.DialogResult.OK==kOpenfiledlg.ShowDialog())
			{
				//����ڰ� ������ �ϴ� �ڽ��� ���� ���� ���
				string kUserSettingFileStr = kOpenfiledlg.FileName;
				//���� �� ���� ���� ���
				string kFullPathofSettingFileStr = GetUISettingfilePath();

				//����ڰ� ������ ui�������� ��ä
				FileInfo kUserSettingFileFI = null;

				try
				{
					kUserSettingFileFI = new FileInfo(kUserSettingFileStr);
					kUserSettingFileFI.OpenRead();
				}
				catch
				{
					MessageBox.Show("������ ����\n" + kUserSettingFileStr + " �� ���� �����ϴ�.");
					return;
				}

				try
				{
					kUserSettingFileFI.CopyTo(kFullPathofSettingFileStr, true);
				}
				catch
				{
					MessageBox.Show("���� ���� ����.");
					return;
				}
				MessageBox.Show("�������� ���翡 �����߽��ϴ�.\n ���α׷�"+
				"����� �� ����˴ϴ�.\n(����)���� ����� UI�� ������� �ʽ��ϴ�!\n"+
				"������ ���Ͻø� File>UISave�� �̿��ϼ���");
				m_bCustomUILoaded = true;
			}
		}
	

		[UICommandHandler("NewFile")]
		private void OnNewFile(object sender, EventArgs args)
		{
			if (!CheckBeforeLosingChanges())
			{				
				return;
			}
			IMessageService messageService = 
				ServiceProvider.Instance.GetService(
				typeof(IMessageService)) as IMessageService;
			messageService.ClearMessages(MessageChannelType.Conflicts);
			MakeNewFile(true);
			m_kElevatorPanel.Clear();
			m_kMonsterPanel.Clear();
			m_kWayPointPanel.Clear();
			m_kFogPanel.Clear();
			MFramework.Instance.ExtEntity.Clear();
			MFramework.Instance.PaletteManager.RemoveAllPalettes();
			MFramework.Instance.PaletteManager.ActivePalette = null;
			MFramework.Instance.PaletteManager.Refresh = true;
		}

		private void MakeNewFile(bool bAskForPalettePath)
		 {
			MScene pmScene = MFramework.Instance.NewScene(0);
			MFramework.Instance.InitNewScene(pmScene);
			string newFilePalettePath = null;
			if (true == bAskForPalettePath)
			{
				newFilePalettePath = AskUserForPalettePath();
			}
			ISettingsService settingService =
				ServiceProvider.Instance.GetService(typeof(ISettingsService))
				as ISettingsService;
			settingService.ScenePath = string.Empty;
			if (null != newFilePalettePath)
			{
				settingService.SetSettingsObject("PaletteFolder", 
					new MFolderLocation(newFilePalettePath),
					SettingsCategory.PerUser);
			}
			SceneManagementUtilities.ClearSceneSettings();
			SceneManagementUtilities.ResetPalettes();
			ISelectionService pmSelectionService =
				ServiceProvider.Instance.GetService(
				typeof(ISelectionService)) as ISelectionService;
			pmSelectionService.ClearSelectedEntities();

			DirtyBitUtilities.MakeSceneClean(MFramework.Instance.Scene);
			this.Text = BuildTitleBarText("Untitled");  
			m_currentFileName = string.Empty;
		}

		private string AskUserForPalettePath()
		{
			// Retrieve setting for whether or not to prompt the user for
			// a palette path.
			ISettingsService pmSettingsService = ServiceProvider.Instance
				.GetService(typeof(ISettingsService)) as ISettingsService;
			Debug.Assert(pmSettingsService != null,
				"Settings service not found!");
			object pmDisplayNewSceneFormObject = pmSettingsService
				.GetSettingsObject(ms_strDisplayNewSceneFormSettingName,
				SettingsCategory.PerUser);
			Debug.Assert(pmDisplayNewSceneFormObject != null, "\"" +
				ms_strDisplayNewSceneFormSettingName + "\" setting not " +
				"found!");
			bool bDisplayNewSceneForm = (bool) pmDisplayNewSceneFormObject;

			string strPalettePath = null;
			if (true == bDisplayNewSceneForm)
			{
				NewSceneForm pmNewSceneForm = new NewSceneForm();
				pmNewSceneForm.ShowDialog();
				strPalettePath = pmNewSceneForm.PalettePath;
			}

			return strPalettePath;
		}

		[UICommandHandler("SaveDefaultConfig")]
		private void SaveConfigToFile(object sender, EventArgs args)
		{
			using (FileStream fs = new FileStream("DefaultDocking.config",
					   FileMode.Create))
			{
				CommandPanelServiceImpl panelService =
					ServiceProvider.Instance.
					GetService(typeof(ICommandPanelService))
					as CommandPanelServiceImpl;
				panelService.SaveConfig(fs);
			}
		}

		[UICommandHandler("ResetWindowLayout")]
		private void OnResetWindowLayout(object sender, EventArgs args)
		{
			DialogResult result = MessageBox.Show(
				"Would you like to reset the window layout?\n" +
				string.Format(
				"The default layout will be restored next time {0} is launched.",
				MFramework.Instance.ApplicationName),
				"Reset Window Layout?", MessageBoxButtons.YesNo,
				MessageBoxIcon.Question);
			m_bClearWindowLayout = (result == DialogResult.Yes);
		}

		[UICommandHandler("Options")]
		private void OnOptions(object sender, EventArgs args)
		{
			
			OptionsDlg dlg = new OptionsDlg();
			dlg.SetCategories( new SettingsCategory[]
				{
						SettingsCategory.Global, SettingsCategory.PerUser, 
					SettingsCategory.PerScene, SettingsCategory.Temp
				});
			dlg.Text = "Options";
			dlg.ShowDialog();
		}

		[UICommandHandler("ClearMRUList")]
		private void OnClearMRUList(object sender, EventArgs args)
		{
			MRUManager.Clear();
		}

		[UICommandHandler("Restart")]
		private void OnRestart(object sender, EventArgs args)
		{
			string path = Assembly.GetEntryAssembly().Location;
			string command = path;
			Mutex mutex = new Mutex(true, "SceneDesignerRestart");
			mutex.WaitOne();
			ProcessStartInfo psi = new ProcessStartInfo(command);
			psi.WorkingDirectory = Path.GetDirectoryName(command);
			System.Diagnostics.Process.Start(psi);
			m_restartLock = mutex;
			this.Close();
		}

		#endregion


		// Shortcut �����ϱ�.
		protected override bool ProcessCmdKey(
			ref System.Windows.Forms.Message msg, Keys keyData)
		{
			AccessibleObject focusedObj = AccessibilityObject.GetFocused();

			if(focusedObj != null &&
				(
				focusedObj.Parent.Name == "�Ӽ� â"
				|| focusedObj.Parent.Name == "Entity Properties"
				|| focusedObj.Parent.Name == "Xml Properties"
				|| focusedObj.Parent.Name == "Monster Area"
				|| focusedObj.Parent.Name == "Selection"
				// Entity Properties
				|| focusedObj.Name == "Name"
				|| focusedObj.Name == "NIF File Path" 
				|| focusedObj.Name == "Postfix Texture"
				// XML Properties
				|| focusedObj.Name == "ActionPath" 
				|| focusedObj.Name == "BGSoundPath" 
				|| focusedObj.Name == "NifPath" 
				|| focusedObj.Name == "ScriptPath"
				|| focusedObj.Name == "WorldID"
				) )
			{
				return false;
			}

			if (!base.ProcessCmdKey (ref msg, keyData))
			{
				return ShortcutService.HandleKey(keyData);
			}
			else
			{
				return true;
			}
		}

		private void m_timerComponent_Tick(object sender, System.EventArgs e)
		{
			//Very lazy execution, since we dont really need this to update
			//we lightning speed, a 1000 ms timer should suffice
			bool bMainSceneDirty = MFramework.Instance.Scene.Dirty;
			CheckAutosaveInterval(bMainSceneDirty);
			if (bMainSceneDirty != m_lastDirtyStatus)
			{								
				if (bMainSceneDirty)
				{
					if (this.Text.IndexOf("*") == -1)
					{
						this.Text = this.Text + "*";
					}
				}
				else
				{
					this.Text = this.Text.Replace("*", "");
				}
				m_lastDirtyStatus = bMainSceneDirty;
			}
		}

		#region Autosaving Methods

		private void CheckAutosaveInterval(bool dirty)
		{
			if (dirty)
			{
				ISettingsService settingsSvc = 
					ServiceProvider.Instance.GetService(
					typeof(ISettingsService)) as ISettingsService;
				int intervalMinutes = (int) 
					settingsSvc.GetSettingsObject("AutoSaveIntervalInMinutes",
					SettingsCategory.PerUser);
				if (intervalMinutes <= 0)
					return;
				TimeSpan interval = new TimeSpan(0, intervalMinutes, 0);

				if ((DateTime.Now - m_timeOfLastAutoSave) > interval)
				{
					DoAutosave();
					m_timeOfLastAutoSave = DateTime.Now;
				}
			}
			else
			{
				m_timeOfLastAutoSave = DateTime.Now;
			}
		}

		private void DoAutosave()
		{
			string autosaveFilename = GetAutosavePath();
			SaveMainScene(autosaveFilename, "GSA", true);
		}

		private void DeleteAutoSave()
		{
			string autosaveFilename = GetAutosavePath();
			FileInfo fi = new FileInfo(autosaveFilename);
			if (fi.Exists)
			{ 
				fi.Delete();
			}
			//Now, delete the settings file
			fi = new FileInfo(autosaveFilename + ".scene.settings");
			if (fi.Exists)
			{
				fi.Delete();
			}
		}

		private bool CheckForAutosaveFiles()
		{
			MFramework fw = MFramework.Instance;
			string autosaveFilename = GetAutosavePath();
			FileInfo fi = new FileInfo(autosaveFilename);
			if (fi.Exists)
			{
				string message = fw.ApplicationName +  
					" was not shut down properly.\nWould you like to " +
					"recover the unsaved file?";
				DialogResult result = MessageBox.Show(message, 
					"Recover File?", MessageBoxButtons.YesNo);
				if (result == DialogResult.Yes)
				{
					LoadMainScene(autosaveFilename, "GSA", true);
					return true;
				}
			}
			return false;
			
		}

		private string GetAutosavePath()
		{
			string folderPath = Environment.GetFolderPath(
				Environment.SpecialFolder.LocalApplicationData) +
				@"\Emergent Game Technologies\" + 
				MFramework.Instance.ApplicationName;

			DirectoryInfo di = new DirectoryInfo(folderPath);
			if (!di.Exists)
			{
				di.Create();
			}

			return folderPath + @"\autosave.gsa";

		}


		#endregion

		private void m_ctlDockPanel_Paint(object sender, PaintEventArgs e)
		{

		}


	}
}
