using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;
using Emergent.Gamebryo.SceneDesigner.Framework;


namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	public partial class ZoneControl : Form
	{
		public ZoneControl()
		{
			InitializeComponent();

			m_cbPlayerBase.Enabled = false;
		}

		private void m_cbPlayerBase_CheckedChanged(object sender, EventArgs e)
		{
			MFramework.Instance.ZoneControl.PlayerBase = m_cbPlayerBase.Checked;
			
			if (null == MFramework.Instance.ZoneControl.GetPlayer())
			{
				MPalette pkPalette = MFramework.Instance.PaletteManager.GetPaletteByName("General");
				if (pkPalette == null)
				{
					MessageBox.Show("General 팔레트가 없습니다.");
					m_cbActivateZoneControl.Checked = false;
					return;
				}
				MEntity pkTemplate = pkPalette.GetEntityByName("[General]Target.player_Anchor");
				if (pkTemplate == null)
				{
					MessageBox.Show("Player entity가 팔레트에 없습니다.");
					m_cbPlayerBase.Checked = false;
					return;
				}
				
				MFramework.Instance.ZoneControl.SetPlayer(pkTemplate.Clone("Zone_Player", true));
				MFramework.Instance.Scene.AddEntity(MFramework.Instance.ZoneControl.GetPlayer(), false);
			}

			if (false != m_cbPlayerBase.Checked)
			{
				MFramework.Instance.ZoneControl.GetPlayer().Hidden = false;
			}
			else
			{
				MFramework.Instance.ZoneControl.GetPlayer().Hidden = true;
			}

			MFramework.Instance.ZoneControl.PlayerBase = m_cbPlayerBase.Checked;
		}

		private void m_cbActivateZoneControl_CheckedChanged(object sender, EventArgs e)
		{
			if (null == MFramework.Instance.ZoneControl.GetAnchor())
			{
				MPalette pkPalette = MFramework.Instance.PaletteManager.GetPaletteByName("General");
				if (pkPalette == null)
				{
					MessageBox.Show("General 팔레트가 없습니다.");
					m_cbActivateZoneControl.Checked = false;
					return;
				}
				MEntity pkTemplate = pkPalette.GetEntityByName("[General]Target.Zone_Anchor");
				if (pkTemplate == null)
				{
					MessageBox.Show("Anchor entity가 팔레트에 없습니다.");
					m_cbActivateZoneControl.Checked = false;
					return;
				}

				MFramework.Instance.ZoneControl.SetAnchor(pkTemplate.Clone("Zone_Anchor", true));
				MFramework.Instance.Scene.AddEntity(MFramework.Instance.ZoneControl.GetAnchor(), false);
			}

			if (false != m_cbActivateZoneControl.Checked)
			{
				m_cbPlayerBase.Enabled = true;
				MFramework.Instance.ZoneControl.GetAnchor().Hidden = false;
			}
			else
			{
				m_cbPlayerBase.Enabled = false;
				MFramework.Instance.ZoneControl.GetAnchor().Hidden = true;
			}	

			MFramework.Instance.ZoneControl.ZoneDraw = m_cbActivateZoneControl.Checked;
		}

		private void m_btnComputeBound_Click(object sender, EventArgs e)
		{
			MFramework.Instance.ZoneControl.ComputeBound();
		}
	}
}