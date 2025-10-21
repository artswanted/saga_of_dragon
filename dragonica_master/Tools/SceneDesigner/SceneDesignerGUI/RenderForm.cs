using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Diagnostics;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.GUI
{
	/// <summary>
	/// Summary description for RenderForm.
	/// </summary>
	[DockPosition(Position=DefaultDock.Document, AllowFloat=false)]
	public class RenderForm : Form
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public RenderForm()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

		}

		public override Cursor Cursor
		{
			get
			{
				return InteractionModeService.ActiveMode.MouseCursor;
			}
			set
			{
				base.Cursor = value;
			}
		}


		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				Invalidator.Instance.UnregisterControl(this);
				if(components != null)
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
			this.SuspendLayout();
			// 
			// RenderForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
			this.ClientSize = new System.Drawing.Size(416, 289);
			this.Name = "RenderForm";
			this.Text = "Viewports";
			this.MouseWheel += new System.Windows.Forms.MouseEventHandler(this.RenderForm_MouseWheel);
			this.MouseEnter += new System.EventHandler(this.RenderForm_MouseEnter);
			this.DoubleClick += new System.EventHandler(this.RenderForm_DoubleClick);
			this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.RenderForm_MouseUp);
			this.MouseLeave += new System.EventHandler(this.RenderForm_MouseLeave);
			this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.RenderForm_MouseMove);
			this.KeyDown += new System.Windows.Forms.KeyEventHandler(this.RenderForm_KeyDown);
			this.MouseHover += new System.EventHandler(this.RenderForm_MouseHover);
			this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.RenderForm_MouseDown);
			this.Load += new System.EventHandler(this.RenderForm_Load);
			this.ResumeLayout(false);

		}
		#endregion


		private bool m_bRendererCreated = false;
		private bool m_bAttemptCreate = true;
		private bool m_bNeedsRecreate = false;

		private ContextMenu m_menuCameras = null;
		private MenuItem m_miSceneCameras = null;
		private MenuItem m_miRenderingModes = null;

		private void InitRenderer()
		{
			if (!MFramework.InstanceIsValid() || m_bRendererCreated ||
				!m_bAttemptCreate)
			{
				return;
			}

			if (!MFramework.Instance.Renderer.Create(this.Handle))
			{
				m_bAttemptCreate = false;
				MessageBox.Show("Renderer creation has failed.",
					"Renderer creation failure!", MessageBoxButtons.OK,
					MessageBoxIcon.Exclamation);
				return;
			}

			m_bRendererCreated = true;
		}

		private void InitContextMenu()
		{
			if (m_menuCameras != null)
			{
				return;
			}

			// Create context menu.
			m_menuCameras = new ContextMenu();
			m_menuCameras.Popup += new EventHandler(m_menuCameras_Popup);
		}

		protected override void OnPaint(PaintEventArgs e)
		{
			if (!MFramework.InstanceIsValid())
			{
				base.OnPaint(e);
			}
			else
			{
				if (!m_bRendererCreated && m_bAttemptCreate)
				{
					InitRenderer();
					InitContextMenu();
				}
				else if (m_bRendererCreated && m_bNeedsRecreate)
				{
					if (MFramework.Instance.Renderer.Recreate())
					{
						m_bNeedsRecreate = false;
					}
				}
				MFramework.Instance.Update();
			}
		}

		protected override void OnPaintBackground(PaintEventArgs pevent)
		{
			if (!MFramework.InstanceIsValid() || !m_bRendererCreated)
			{
				base.OnPaintBackground(pevent);
			}
		}
	
		protected override void OnResize(EventArgs e)
		{
			if (MFramework.InstanceIsValid())
			{
				m_bNeedsRecreate = true;
			}

			base.OnResize(e);
		}

		private IInteractionMode.MouseButtonType ConvertMouseButton(
			System.Windows.Forms.MouseButtons button)
		{
			switch (button)
			{
				case MouseButtons.Left:
					return IInteractionMode.MouseButtonType.LeftButton;
				case MouseButtons.Middle:
					return IInteractionMode.MouseButtonType.MiddleButton;
				case MouseButtons.Right:
					return IInteractionMode.MouseButtonType.RightButton;
				default:
					// Unknown mouse button.
					throw new FormatException();
			};
		}

		private bool MouseIsOverCameraLabel(int iViewportX, int iViewportY)
		{
			
			Rectangle mRect = MFramework.Instance.ViewportManager
				.ActiveViewport.CameraNameRect;
			return mRect.Contains(iViewportX, iViewportY);
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

		private static IRenderingModeService m_pmRenderingModeService;
		private static IRenderingModeService RenderingModeService
		{
			get
			{
				if (m_pmRenderingModeService == null)
				{
					m_pmRenderingModeService = ServiceProvider.Instance
						.GetService(typeof(IRenderingModeService)) as
						IRenderingModeService;
					Debug.Assert(m_pmRenderingModeService != null,
						"Rendering mode service not found!");
				}
				return m_pmRenderingModeService;
			}
		}

		private void RenderForm_MouseEnter(object sender, System.EventArgs e)
		{
			if (MFramework.InstanceIsValid() &&
				InteractionModeService.ActiveMode != null)
			{
				InteractionModeService.ActiveMode.MouseEnter();
			}
		}

		private void RenderForm_MouseLeave(object sender, System.EventArgs e)
		{
			if (MFramework.InstanceIsValid() &&
				InteractionModeService.ActiveMode != null)
			{
				InteractionModeService.ActiveMode.MouseLeave();
			}
		}

		private void RenderForm_MouseHover(object sender, System.EventArgs e)
		{
			if (MFramework.InstanceIsValid() &&
				InteractionModeService.ActiveMode != null)
			{
				InteractionModeService.ActiveMode.MouseHover();
			}
		}

		private void RenderForm_MouseDown(object sender,
			System.Windows.Forms.MouseEventArgs e)
		{
			this.Activate();
			this.Focus();
			if (MFramework.InstanceIsValid())
			{
				MViewportManager pmViewportManager = 
					MFramework.Instance.ViewportManager;
				pmViewportManager.ActivateViewport(e.X, e.Y);

				int iViewportX, iViewportY;
				pmViewportManager
					.ScreenCoordinatesToViewportCoordinates(e.X, e.Y,
					out iViewportX, out iViewportY);

				if (e.Button == MouseButtons.Right &&
					MouseIsOverCameraLabel(e.X, e.Y))
				{
					m_menuCameras.Show(this, new Point(e.X, e.Y));
					return;
				}

				if (InteractionModeService.ActiveMode != null)
				{
					try
					{
						if (pmViewportManager.IsInsideActiveViewport(e.X, e.Y))
						{
                            //if (!MFramework.Instance.CameraManager.IsSceneCamera())//!/
                            {
                                InteractionModeService.ActiveMode.MouseDown(
                                    ConvertMouseButton(e.Button), iViewportX,
                                    iViewportY);
                            }
						}
					}
					catch (FormatException)
					{
						// Unknown mouse button; ignore.
					}
				}
			}
		}

		private void RenderForm_MouseUp(object sender,
			System.Windows.Forms.MouseEventArgs e)
		{
			if (MFramework.InstanceIsValid() &&
				InteractionModeService.ActiveMode != null)
			{
				int iViewportX, iViewportY;
				MFramework.Instance.ViewportManager
					.ScreenCoordinatesToViewportCoordinates(e.X, e.Y,
					out iViewportX, out iViewportY);
				try
				{
					InteractionModeService.ActiveMode.MouseUp(
						ConvertMouseButton(e.Button), iViewportX, iViewportY);
				}
				catch (FormatException)
				{
					// Unknown mouse button; ignore.
				}
			}
		}

		private void RenderForm_MouseMove(object sender,
			System.Windows.Forms.MouseEventArgs e)
		{
			if (MFramework.InstanceIsValid() &&
				InteractionModeService.ActiveMode != null)
			{
				int iViewportX, iViewportY;
				MFramework.Instance.ViewportManager
					.ScreenCoordinatesToViewportCoordinates(e.X, e.Y,
					out iViewportX, out iViewportY);
				InteractionModeService.ActiveMode.MouseMove(iViewportX,
					iViewportY);
			}
		}

		private void RenderForm_MouseWheel(object sender,
			System.Windows.Forms.MouseEventArgs e)
		{
			if (MFramework.InstanceIsValid() &&
				InteractionModeService.ActiveMode != null)
			{
				InteractionModeService.ActiveMode.MouseWheel(e.Delta);
			}
		}

		private void RenderForm_DoubleClick(object sender, System.EventArgs e)
		{
			if (MFramework.InstanceIsValid() &&
				InteractionModeService.ActiveMode != null)
			{
				InteractionModeService.ActiveMode.DoubleClick();
			}
		}

		private void RenderForm_KeyDown(object sender, 
			System.Windows.Forms.KeyEventArgs e)
		{
			string strName = null;
			bool bChangeCamera = false;
			bool bChangeSceneCamera = false;
			switch (e.KeyData)
			{
				case Keys.Delete:
				{
					e.Handled = true;
					IUICommandService uiCommandService = 
						ServiceProvider.Instance.GetService(
							typeof(IUICommandService)) as IUICommandService;
					UICommand command = 
						uiCommandService.GetCommand("DeleteSelectedEntities");
					UIState state = new UIState();
					command.ValidateCommand(state);
					if (state.Enabled)
					{
						command.DoClick(command, null);
					}
					break;
				}
				case Keys.P:
				{
					strName = "Perspective";
					bChangeCamera = true;
					break;
				}
				case Keys.U:
				{
					strName = "User";
					bChangeCamera = true;
					break;
				}
				case Keys.L:
				{
					strName = "Left_PositiveX";
					bChangeCamera = true;
					break;
				}
				case Keys.F:
				{
					strName = "Front_PositiveY";
					bChangeCamera = true;
					break;
				}
				case Keys.B:
				{
					strName = "Bottom_PositiveZ";
					bChangeCamera = true;
					break;
				}
				case Keys.T:
				{
					strName = "Top_NegativeZ";
					bChangeCamera = true;
					break;
				}
				case Keys.F2:
				{
					strName = "Standard";
					bChangeSceneCamera = true;
					break;
				}
				case Keys.F3:
				{
					strName = "Wireframe (Shaded)";
					bChangeSceneCamera = true;
					break;
				}
				case Keys.F4:
				{
					strName = "Wireframe (Unshaded)";
					bChangeSceneCamera = true;
					break;
				}
		   }

		   MCameraManager pmCameraManager = MFramework.Instance
			   .CameraManager;
		   MViewport pmActiveViewport = MFramework.Instance.ViewportManager
			   .ActiveViewport;
		   if (bChangeCamera && strName != null)
		   {
				MCameraManager.StandardCamera eCameraType =
					(MCameraManager.StandardCamera)Enum.Parse(typeof(
					MCameraManager.StandardCamera), strName);
				MEntity pmStandardCamera = pmCameraManager
					.GetStandardCameraEntity(pmActiveViewport, eCameraType);
				pmCameraManager.TransitionViewportToCamera(
					pmActiveViewport, pmStandardCamera);
			}

			if (bChangeSceneCamera && strName != null)
			{
				foreach (IRenderingMode pmRenderingMode in
					RenderingModeService.GetRenderingModes())
				{
					if (pmRenderingMode.Name.Equals(strName) &&
						pmActiveViewport.RenderingMode != pmRenderingMode)
					{
						pmActiveViewport.RenderingMode = pmRenderingMode;
						break;
					}
				}
			}
		}

		private void m_menuCameras_Popup(object sender, System.EventArgs e)
		{
			// Clear all menu items.
			m_menuCameras.MenuItems.Clear();

			// Build sorted array of rendering mode names.
			IRenderingMode[] amRenderingModes = RenderingModeService
				.GetRenderingModes();
			ArrayList pmRenderingModeNames = new ArrayList(
				amRenderingModes.Length);
			foreach (IRenderingMode pmRenderingMode in amRenderingModes)
			{
				if (pmRenderingMode.DisplayToUser)
				{
					pmRenderingModeNames.Add(pmRenderingMode.Name);
				}
			}
			pmRenderingModeNames.Sort();

			// Populate rendering modes menu.
			if (m_miRenderingModes != null)
			{
				m_menuCameras.MenuItems.Remove(m_miRenderingModes);
				m_miRenderingModes = null;
			}
			if (pmRenderingModeNames.Count > 0)
			{
				m_miRenderingModes = new MenuItem("Rendering Modes");

				foreach (string strRenderingModeName in pmRenderingModeNames)
				{
					m_miRenderingModes.MenuItems.Add(strRenderingModeName,
						new EventHandler(OnMenuItemClick));
				}

				m_menuCameras.MenuItems.Add(m_miRenderingModes);
			}

			// Check the menu item for the current rendering mode.
			IRenderingMode pmActiveRenderingMode = MFramework.Instance
				.ViewportManager.ActiveViewport.RenderingMode;
			if (pmActiveRenderingMode != null)
			{
				foreach (MenuItem pmItem in m_miRenderingModes.MenuItems)
				{
					if (pmItem.Text.Equals(pmActiveRenderingMode.Name))
					{
						pmItem.Checked = true;
					}
					else
					{
						pmItem.Checked = false;
					}
				}
			}

			// Add separator.
			m_menuCameras.MenuItems.Add("-");

			// Add menu items for standard cameras.
			foreach (string strCameraName in Enum.GetNames(typeof(
				MCameraManager.StandardCamera)))
			{
				m_menuCameras.MenuItems.Add(strCameraName, new EventHandler(
					OnMenuItemClick));
			}

			// Build sorted array of scene camera names.
			MCameraManager pmCameraManager = MFramework.Instance
				.CameraManager;
			ArrayList pmCameraNames = new ArrayList((int)pmCameraManager
				.SceneCameraCount);
			for (uint ui = 0; ui < pmCameraManager.SceneCameraCount; ui++)
			{
				pmCameraNames.Add(pmCameraManager.GetSceneCameraEntity(ui)
					.Name);
			}
			pmCameraNames.Sort();

			// Populate scene cameras menu.
			if (m_miSceneCameras != null)
			{
				m_menuCameras.MenuItems.Remove(m_miSceneCameras);
				m_miSceneCameras = null;
			}
			if (pmCameraNames.Count > 0)
			{
				m_miSceneCameras = new MenuItem("Scene Cameras");

				foreach (string strCameraName in pmCameraNames)
				{
					m_miSceneCameras.MenuItems.Add(strCameraName, new
						EventHandler(OnMenuItemClick));
				}

				m_menuCameras.MenuItems.Add(m_miSceneCameras);
			}

			// Check the menu item for the current viewport camera.
			MEntity pmActiveCamera = MFramework.Instance.ViewportManager
				.ActiveViewport.CameraEntity;
			foreach (MenuItem pmItem in m_menuCameras.MenuItems)
			{
				if (pmItem.Text.Equals(pmActiveCamera.Name))
				{
					pmItem.Checked = true;
				}
				else
				{
					pmItem.Checked = false;
				}
			}
			if (m_miSceneCameras != null)
			{
				foreach (MenuItem pmItem in m_miSceneCameras.MenuItems)
				{
					if (pmItem.Text.Equals(pmActiveCamera.Name))
					{
						pmItem.Checked = true;
					}
					else
					{
						pmItem.Checked = false;
					}
				}
			}


			// Add separator.
			m_menuCameras.MenuItems.Add("-");
		}

		private void OnMenuItemClick(object sender, System.EventArgs e)
		{
			MenuItem pmItem = sender as MenuItem;
			MCameraManager pmCameraManager = MFramework.Instance
				.CameraManager;
			MViewport pmActiveViewport = MFramework.Instance.ViewportManager
				.ActiveViewport;

			// Activate the clicked camera on the active viewport.
			try
			{
				// Find standard camera clicked.
				MCameraManager.StandardCamera eCameraType =
					(MCameraManager.StandardCamera) Enum.Parse(typeof(
					MCameraManager.StandardCamera), pmItem.Text);
				MEntity pmStandardCamera = pmCameraManager
					.GetStandardCameraEntity(pmActiveViewport, eCameraType);
				if (pmActiveViewport.CameraEntity != pmStandardCamera)
				{
					pmCameraManager.TransitionViewportToCamera(
						pmActiveViewport, pmStandardCamera);
				}
			}
			catch (ArgumentException)
			{
				bool bFinished = false;

				// Find scene camera clicked.
				for (uint ui = 0; ui < pmCameraManager.SceneCameraCount; ui++)
				{
					MEntity pmSceneCamera = pmCameraManager
						.GetSceneCameraEntity(ui);
					if (pmSceneCamera.Name.Equals(pmItem.Text) &&
						pmActiveViewport.CameraEntity != pmSceneCamera)
					{
						pmCameraManager.TransitionViewportToCamera(
							pmActiveViewport, pmSceneCamera);
						bFinished = true;
						break;
					}
				}

				// Find rendering mode clicked.
				if (!bFinished)
				{
					foreach (IRenderingMode pmRenderingMode in
						RenderingModeService.GetRenderingModes())
					{
						if (pmRenderingMode.Name.Equals(pmItem.Text) &&
							pmActiveViewport.RenderingMode != pmRenderingMode)
						{
							pmActiveViewport.RenderingMode = pmRenderingMode;
							bFinished = true;
							break;
						}
					}
				}
			}
		}

		private void RenderForm_Load(object sender, EventArgs e)
		{

		}
	}
}
