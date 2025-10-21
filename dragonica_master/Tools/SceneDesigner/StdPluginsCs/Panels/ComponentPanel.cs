// EMERGENT GAME TECHNOLOGIES PROPRIETARY INFORMATION
//
// This software is supplied under the terms of a license agreement or
// nondisclosure agreement with Emergent Game Technologies and may not 
// be copied or disclosed except in accordance with the terms of that 
// agreement.
//
//      Copyright (c) 1996-2006 Emergent Game Technologies.
//      All Rights Reserved.
//
// Emergent Game Technologies, Chapel Hill, North Carolina 27517
// http://www.emergent.net

using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;
using System.Diagnostics;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;
using Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Dialogs;
using Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Utility;

namespace Emergent.Gamebryo.SceneDesigner.StdPluginsCs.Panels
{
	public class ComponentPanel : Form, ICommandPanel
	{
		#region Private Data
		private IComponentService m_componentService;
		private IUICommandService m_uiCommandService;
		private ICommandService m_commandService;
		MComponent m_selectedComponent;
		#endregion

		private System.Windows.Forms.Label m_lblPropertySets;
		private System.Windows.Forms.ComboBox m_cbPropertySets;
		private System.Windows.Forms.Label m_lblProperties;
		private System.Windows.Forms.PropertyGrid m_gridProperties;
		private System.Windows.Forms.Button m_btnAddProperty;
		private System.Windows.Forms.Label m_lblDummy;
		private System.Windows.Forms.Button m_btnCreateNewComponent;
		private System.Windows.Forms.Button m_btnEditProperty;
		private System.Windows.Forms.Button m_btnDeleteProperty;
		private System.ComponentModel.IContainer components = null;

		public ComponentPanel()
		{
			// This call is required by the Windows Form Designer.
			InitializeComponent();

			UICommandService.BindCommands(this);
		}

		class ComponentWrapper
		{
			public MComponent Component;
			public override string ToString()
			{
				return Component.Name;
			}

		}

		private IComponentService ComponentService
		{
			get
			{
				if (m_componentService == null)
				{
					ServiceProvider sp = ServiceProvider.Instance;
					m_componentService =
						sp.GetService(typeof(IComponentService))
						as IComponentService;
				}
				return m_componentService;
			}
		}

		private IUICommandService UICommandService
		{
			get
			{
				if (m_uiCommandService == null)
				{
					ServiceProvider sp = ServiceProvider.Instance;
					m_uiCommandService =
						sp.GetService(typeof(IUICommandService))
						as IUICommandService;
				}
				return m_uiCommandService;
			}
		}

		private ICommandService CommandService
		{
			get
			{
				if (m_commandService == null)
				{
					ServiceProvider sp = ServiceProvider.Instance;
					m_commandService = sp.GetService(typeof(ICommandService))
						as ICommandService;
				}
				return m_commandService;
			}

		}

		private MFramework FW
		{
			get { return MFramework.Instance; }
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose(bool disposing)
		{
			if (disposing)
			{
				if (components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose(disposing);
		}

		#region Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(ComponentPanel));
			this.m_lblPropertySets = new System.Windows.Forms.Label();
			this.m_cbPropertySets = new System.Windows.Forms.ComboBox();
			this.m_lblProperties = new System.Windows.Forms.Label();
			this.m_gridProperties = new System.Windows.Forms.PropertyGrid();
			this.m_btnAddProperty = new System.Windows.Forms.Button();
			this.m_btnCreateNewComponent = new System.Windows.Forms.Button();
			this.m_lblDummy = new System.Windows.Forms.Label();
			this.m_btnEditProperty = new System.Windows.Forms.Button();
			this.m_btnDeleteProperty = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// m_lblPropertySets
			// 
			this.m_lblPropertySets.AutoSize = true;
			this.m_lblPropertySets.Location = new System.Drawing.Point(10, 9);
			this.m_lblPropertySets.Name = "m_lblPropertySets";
			this.m_lblPropertySets.Size = new System.Drawing.Size(127, 12);
			this.m_lblPropertySets.TabIndex = 0;
			this.m_lblPropertySets.Text = "Selected Component:";
			// 
			// m_cbPropertySets
			// 
			this.m_cbPropertySets.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.m_cbPropertySets.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
			this.m_cbPropertySets.Location = new System.Drawing.Point(10, 34);
			this.m_cbPropertySets.Name = "m_cbPropertySets";
			this.m_cbPropertySets.Size = new System.Drawing.Size(350, 20);
			this.m_cbPropertySets.Sorted = true;
			this.m_cbPropertySets.TabIndex = 1;
			this.m_cbPropertySets.SelectionChangeCommitted += new System.EventHandler(this.m_cbPropertySets_SelectionChangeCommitted);
			this.m_cbPropertySets.SelectedIndexChanged += new System.EventHandler(this.m_cbPropertySets_SelectedIndexChanged);
			// 
			// m_lblProperties
			// 
			this.m_lblProperties.AutoSize = true;
			this.m_lblProperties.Location = new System.Drawing.Point(10, 69);
			this.m_lblProperties.Name = "m_lblProperties";
			this.m_lblProperties.Size = new System.Drawing.Size(66, 12);
			this.m_lblProperties.TabIndex = 2;
			this.m_lblProperties.Text = "Properties:";
			// 
			// m_gridProperties
			// 
			this.m_gridProperties.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
						| System.Windows.Forms.AnchorStyles.Left)
						| System.Windows.Forms.AnchorStyles.Right)));
			this.m_gridProperties.LineColor = System.Drawing.SystemColors.ScrollBar;
			this.m_gridProperties.Location = new System.Drawing.Point(10, 86);
			this.m_gridProperties.Name = "m_gridProperties";
			this.m_gridProperties.PropertySort = System.Windows.Forms.PropertySort.Alphabetical;
			this.m_gridProperties.Size = new System.Drawing.Size(350, 245);
			this.m_gridProperties.TabIndex = 3;
			this.m_gridProperties.ToolbarVisible = false;
			// 
			// m_btnAddProperty
			// 
			this.m_btnAddProperty.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.m_btnAddProperty.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.m_btnAddProperty.Location = new System.Drawing.Point(12, 337);
			this.m_btnAddProperty.Name = "m_btnAddProperty";
			this.m_btnAddProperty.Size = new System.Drawing.Size(48, 24);
			this.m_btnAddProperty.TabIndex = 4;
			this.m_btnAddProperty.Text = "Add";
			this.m_btnAddProperty.Click += new System.EventHandler(this.m_btnAddProperty_Click);
			// 
			// m_btnCreateNewComponent
			// 
			this.m_btnCreateNewComponent.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.m_btnCreateNewComponent.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.m_btnCreateNewComponent.Location = new System.Drawing.Point(12, 367);
			this.m_btnCreateNewComponent.Name = "m_btnCreateNewComponent";
			this.m_btnCreateNewComponent.Size = new System.Drawing.Size(115, 24);
			this.m_btnCreateNewComponent.TabIndex = 7;
			this.m_btnCreateNewComponent.Text = "New Component";
			this.m_btnCreateNewComponent.Click += new System.EventHandler(this.m_btnCreateNewComponent_Click);
			// 
			// m_lblDummy
			// 
			this.m_lblDummy.Location = new System.Drawing.Point(307, 233);
			this.m_lblDummy.Name = "m_lblDummy";
			this.m_lblDummy.Size = new System.Drawing.Size(10, 8);
			this.m_lblDummy.TabIndex = 8;
			// 
			// m_btnEditProperty
			// 
			this.m_btnEditProperty.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.m_btnEditProperty.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.m_btnEditProperty.Location = new System.Drawing.Point(69, 337);
			this.m_btnEditProperty.Name = "m_btnEditProperty";
			this.m_btnEditProperty.Size = new System.Drawing.Size(48, 24);
			this.m_btnEditProperty.TabIndex = 5;
			this.m_btnEditProperty.Text = "Edit";
			this.m_btnEditProperty.Click += new System.EventHandler(this.m_btnEditProperty_Click);
			// 
			// m_btnDeleteProperty
			// 
			this.m_btnDeleteProperty.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left)));
			this.m_btnDeleteProperty.FlatStyle = System.Windows.Forms.FlatStyle.System;
			this.m_btnDeleteProperty.Location = new System.Drawing.Point(127, 337);
			this.m_btnDeleteProperty.Name = "m_btnDeleteProperty";
			this.m_btnDeleteProperty.Size = new System.Drawing.Size(57, 24);
			this.m_btnDeleteProperty.TabIndex = 6;
			this.m_btnDeleteProperty.Text = "Delete";
			this.m_btnDeleteProperty.Click += new System.EventHandler(this.m_btnDeleteProperty_Click);
			// 
			// ComponentPanel
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(6, 14);
			this.AutoScroll = true;
			this.ClientSize = new System.Drawing.Size(396, 418);
			this.Controls.Add(this.m_btnDeleteProperty);
			this.Controls.Add(this.m_btnEditProperty);
			this.Controls.Add(this.m_btnCreateNewComponent);
			this.Controls.Add(this.m_btnAddProperty);
			this.Controls.Add(this.m_gridProperties);
			this.Controls.Add(this.m_lblProperties);
			this.Controls.Add(this.m_lblPropertySets);
			this.Controls.Add(this.m_cbPropertySets);
			this.Controls.Add(this.m_lblDummy);
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.Name = "ComponentPanel";
			this.Text = "Components";
			this.Load += new System.EventHandler(this.ComponentPanel_Load);
			this.ResumeLayout(false);
			this.PerformLayout();

		}

		private void m_cbPropertySets_SelectionChangeCommitted(object sender,
			System.EventArgs e)
		{
		}

		private void m_cbPropertySets_SelectedIndexChanged(object sender,
			System.EventArgs e)
		{
			ComponentWrapper wrapper =
				m_cbPropertySets.Items[
					m_cbPropertySets.SelectedIndex] as ComponentWrapper;
			m_gridProperties.SelectedObject = wrapper.Component;
			m_selectedComponent = wrapper.Component;
		}

		#endregion

		#region Winforms Handlers

		private void m_btnAddProperty_Click(object sender, System.EventArgs e)
		{
			AddPropertyDlg pmDialog = new AddPropertyDlg();
			if (pmDialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
			{
				ComponentWrapper pmWrapper = m_cbPropertySets.Items[
					m_cbPropertySets.SelectedIndex] as ComponentWrapper;
				MComponent pmComponent = pmWrapper.Component;

				bool bCanAdd = ComponentPropertyUtilities.CanAddProperty(
					pmDialog.NewPropertyName, pmComponent);

				if (bCanAdd)
				{
					CommandService.BeginUndoFrame(string.Format(
						"Add \"{0}\" property to \"{1}\" component",
						pmDialog.NewPropertyName, pmComponent.Name));

					pmComponent.AddProperty(pmDialog.NewPropertyName,
						pmDialog.NewPropertyName,
						pmDialog.NewPropertyType.PrimitiveType,
						pmDialog.NewPropertyType.Name,
						pmDialog.Collection,
						pmDialog.NewPropertyDescription, true);

					RefreshDropDown();
					UpdatePalettesOnAddProperty(pmComponent,
						pmDialog.NewPropertyName);

					CommandService.EndUndoFrame(true);
				}
				else
				{
					MessageBox.Show("The property cannot be added because " +
						"doing so would\nconflict with existing entities " +
						"or palettes.", "Error Adding Property",
						MessageBoxButtons.OK, MessageBoxIcon.Error);
				}
			}
		}

		private void m_btnEditProperty_Click(object sender,
			System.EventArgs e)
		{
			MEntityPropertyDescriptor pmDescriptor = null;
			if (m_gridProperties.SelectedGridItem != null)
			{
				pmDescriptor = m_gridProperties.SelectedGridItem
					.PropertyDescriptor as MEntityPropertyDescriptor;
			}
			if (pmDescriptor == null)
			{
				return;
			}

			string strPropertyName = pmDescriptor.PropertyName;
			PropertyType pmPropertyType = m_selectedComponent.GetPropertyType(
				strPropertyName);
			Debug.Assert(pmPropertyType != null, "Property type not found!");

			AddPropertyDlg pmDialog = new AddPropertyDlg();
			pmDialog.NewPropertyName = strPropertyName;
			pmDialog.NewPropertyType = pmPropertyType;
			pmDialog.Collection =
				m_selectedComponent.IsCollection(strPropertyName);
			pmDialog.NewPropertyDescription = m_selectedComponent
				.GetDescription(strPropertyName);
			pmDialog.Text = "Edit Property";
			if (pmDialog.ShowDialog() == System.Windows.Forms.DialogResult.OK)
			{
				ComponentWrapper pmWrapper = m_cbPropertySets.Items[
					m_cbPropertySets.SelectedIndex] as ComponentWrapper;
				string strComponentName = pmWrapper.ToString();
				MComponent pmComponent = pmWrapper.Component;


				if (!strPropertyName.Equals(pmDialog.NewPropertyName))
				{
					// We are changing the name; check for conflicts.
					bool bCanAdd = ComponentPropertyUtilities.CanAddProperty(
						pmDialog.NewPropertyName, pmComponent);
					bool bCanRemove = ComponentPropertyUtilities
						.CanRemoveProperty(strPropertyName, pmComponent);
					if (!bCanRemove)
					{
						MessageBox.Show("The property cannot be renamed " +
							"because existing\nentities or palettes " +
							"depend on it.", "Error Renaming Property",
							MessageBoxButtons.OK, MessageBoxIcon.Error);
						return;

					}
					if (!bCanAdd)
					{
						MessageBox.Show("The property cannot be renamed " +
							"because it would\nconflict with existing " +
							"entities or palettes", "Error Renaming Property",
							MessageBoxButtons.OK, MessageBoxIcon.Error);
						return;

					}
				}

				CommandService.BeginUndoFrame(string.Format(
					"Changed \"{0}\" property in \"{1}\" component",
					pmDialog.NewPropertyName, strComponentName));

				pmComponent.RemoveProperty(strPropertyName, true);

				UpdatePalettesOnRemoveProperty(pmComponent,
					strPropertyName);

				pmComponent.AddProperty(pmDialog.NewPropertyName,
					pmDialog.NewPropertyName,
					pmDialog.NewPropertyType.PrimitiveType,
					pmDialog.NewPropertyType.Name,
					pmDialog.Collection,
					pmDialog.NewPropertyDescription, true);
				pmComponent.SetPropertyData(pmDialog.NewPropertyName, null,
					false);
				UpdatePalettesOnAddProperty(pmComponent,
					pmDialog.NewPropertyName);
				RefreshDropDown();

				CommandService.EndUndoFrame(true);
			}
		}

		private void m_btnDeleteProperty_Click(object sender,
			System.EventArgs e)
		{
			MEntityPropertyDescriptor pmDescriptor = null;
			if (m_gridProperties.SelectedGridItem != null)
			{
				pmDescriptor = m_gridProperties.SelectedGridItem
					.PropertyDescriptor as MEntityPropertyDescriptor;
			}
			if (pmDescriptor == null)
			{
				return;
			}

			string strPropertyName = pmDescriptor.PropertyName;
			ComponentWrapper pmWrapper = m_cbPropertySets.Items[
				m_cbPropertySets.SelectedIndex] as ComponentWrapper;
			bool bCanRemove = ComponentPropertyUtilities.CanRemoveProperty(
				strPropertyName, pmWrapper.Component);
			if (bCanRemove)
			{
				if (MessageBox.Show("Are you sure you wish to delete the \""
					+ strPropertyName + "\" property?",
					"Delete Property Confirmation", MessageBoxButtons.YesNo,
					MessageBoxIcon.Question) == DialogResult.Yes)
				{
					CommandService.BeginUndoFrame(string.Format(
						"Delete \"{0}\" property in \"{1}\" component",
						strPropertyName, pmWrapper.Component.Name));

					pmWrapper.Component.RemoveProperty(strPropertyName, true);
					UpdatePalettesOnRemoveProperty(pmWrapper.Component,
						strPropertyName);
					RefreshDropDown();

					CommandService.EndUndoFrame(true);
				}
			}
			else
			{
				MessageBox.Show(
					"The property cannot be removed because existing\n" +
					"entities or palettes depend on it.",
					"Error Deleting Property", MessageBoxButtons.OK,
					MessageBoxIcon.Error);
			}
		}

		private void m_btnCreateNewComponent_Click(object sender,
			System.EventArgs e)
		{

			AddNewComponentDlg dlg = new AddNewComponentDlg(
				ComponentService.GetComponentNames());
			if (dlg.ShowDialog() == DialogResult.OK)
			{
				MComponent component = MComponent.CreateGeneralComponent(
					dlg.NewComponentName);

				ComponentService.RegisterComponent(component);
				m_selectedComponent = component;
				RefreshDropDown();
			}

		}

		private void ComponentPanel_Load(object sender, System.EventArgs e)
		{
			RefreshDropDown();
		}

		#endregion

		#region Helper Methods

		private void UpdatePalettesOnAddProperty(MComponent component,
			string propertyName)
		{
			MPalette[] palettes = FW.PaletteManager.GetPalettes();
			MScene[] affectedScenes = new MScene[] { FW.Scene };
			foreach (MPalette palette in palettes)
			{
				palette.AddComponentPropertyToEntities(component,
					propertyName, affectedScenes);
			}
		}
		private void UpdatePalettesOnRemoveProperty(MComponent component,
			string propertyName)
		{
			MPalette[] palettes = FW.PaletteManager.GetPalettes();
			MScene[] affectedScenes = new MScene[] { FW.Scene };
			foreach (MPalette palette in palettes)
			{
				palette.RemoveComponentPropertyFromEntities(component,
					propertyName, affectedScenes);
			}
		}

		private void RefreshDropDown()
		{
			m_cbPropertySets.Items.Clear();
			MComponent[] components = ComponentService.GetAllComponents();
			foreach (MComponent component in components)
			{
				ComponentWrapper wrapper = new ComponentWrapper();
				wrapper.Component = component;
				int index = m_cbPropertySets.Items.Add(wrapper);
				if (m_selectedComponent != null)
				{
					if (component.TemplateID.Equals(
						m_selectedComponent.TemplateID))
					{
						m_cbPropertySets.SelectedIndex = index;
					}
				}
			}
		}


		[UICommandHandler("Idle")]
		private void OnIdle(object sender, EventArgs args)
		{
			bool propertiesCanBeEdited = m_selectedComponent != null &&
				m_selectedComponent.IsAddOrRemovePropertySupported();
			bool subItemSelected =
				m_gridProperties.SelectedGridItem != null &&
				m_gridProperties.SelectedGridItem.Parent !=
				null;
			m_btnAddProperty.Enabled = propertiesCanBeEdited;
			m_btnEditProperty.Enabled = propertiesCanBeEdited &&
				subItemSelected;
			m_btnDeleteProperty.Enabled = propertiesCanBeEdited &&
				subItemSelected;

		}


		#endregion

		#region ICommandPanel Members

		public void RegisterEventHandlers()
		{
			FW.EventManager.ComponentPropertyAdded +=
				new MEventManager.__Delegate_ComponentPropertyAdded(
				EventManager_ComponentPropertyAdded);
			FW.EventManager.ComponentPropertyRemoved +=
				new MEventManager.__Delegate_ComponentPropertyRemoved(
				EventManager_ComponentPropertyRemoved);
			FW.EventManager.ComponentPropertyChanged +=
				new MEventManager.__Delegate_ComponentPropertyChanged(
				EventManager_ComponentPropertyChanged);
			FW.EventManager.ComponentServiceChanged +=
				new MEventManager.__Delegate_ComponentServiceChanged(
				EventManager_ComponentServiceChanged);

		}

		#endregion

		private void EventManager_ComponentPropertyAdded(
			MComponent pmComponent, string strPropertyName, bool bInBatch)
		{
			if (pmComponent == m_selectedComponent)
			{
				m_gridProperties.Refresh();
			}
		}

		private void EventManager_ComponentPropertyRemoved(
			MComponent pmComponent, string strPropertyName, bool bInBatch)
		{
			if (pmComponent == m_selectedComponent)
			{
				m_gridProperties.Refresh();
			}

		}

		private void EventManager_ComponentPropertyChanged(
			MComponent pmComponent, string strPropertyName, 
            uint uiPropertyIndex, bool bInBatch)
		{
			if (pmComponent == m_selectedComponent)
			{
				m_gridProperties.Refresh();
			}

		}

		private void EventManager_ComponentServiceChanged(
			MComponent pmComponent)
		{
			if (!this.IsDisposed || !this.Disposing)
			{
				RefreshDropDown();
				m_gridProperties.Refresh();
			}
		}
	}
}

