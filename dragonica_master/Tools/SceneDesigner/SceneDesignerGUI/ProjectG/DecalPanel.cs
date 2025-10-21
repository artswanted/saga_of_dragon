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
    public partial class DecalPanel : Form
    {
        #region Private Data
        private IUICommandService m_uiCommandService;
        #endregion

        public DecalPanel()
        {
            InitializeComponent();
            m_tbWidth.Text = "10";
            m_tbHeight.Text = "10";
            m_tbDepth.Text = "10";
        }

        private IUICommandService UICommandService
        {
            get
            {
                if (m_uiCommandService == null)
                {
                    m_uiCommandService =
                        ServiceProvider.Instance.GetService(
                        typeof(IUICommandService))
                        as IUICommandService;
                }
                return m_uiCommandService;
            }
        }

        private void TargeterCreate_Click(object sender, EventArgs e)
        {
            MPalette pkPalette = MFramework.Instance.PaletteManager.GetPaletteByName("General");
            if (pkPalette == null)
            {
                MessageBox.Show("General 팔레트가 없습니다.");
                return;
            }
            MEntity pkTemplate = pkPalette.GetEntityByName("[General]Target.decal_target");
            if (pkTemplate == null)
            {
                MessageBox.Show("[General]Target.monster_target 가 없습니다.");
                return;
            }

            MFramework.Instance.PaletteManager.ActivePalette = pkPalette;
            if (MFramework.Instance.PaletteManager.ActivePalette.ActiveEntity !=
                pkTemplate)
            {
                MFramework.Instance.PaletteManager.ActivePalette.ActiveEntity = pkTemplate;
            }

            UICommand command = UICommandService.GetCommand(
                "CreateInteractionMode");
            if (command != null)
            {
                command.DoClick(this, null);
            }
            //////////////////////////////////////////////////////////////////////////
        }

        private void DecalCreate_Click(object sender, EventArgs e)
        {
            float fTemp;
            float.TryParse(m_tbWidth.Text, out fTemp);
            MFramework.Instance.DecalManager.SetWidth(fTemp);
            float.TryParse(m_tbHeight.Text, out fTemp);
            MFramework.Instance.DecalManager.SetHeight(fTemp);
            float.TryParse(m_tbDepth.Text, out fTemp);
            MFramework.Instance.DecalManager.SetDepth(fTemp);
            MFramework.Instance.DecalManager.MakeDecal();
        }

        private void LoadDecal_Click(object sender, EventArgs e)
        {
            MFramework.Instance.DecalManager.LoadDecal();
        }

        private void SaveDecal_Click(object sender, EventArgs e)
        {
            MFramework.Instance.DecalManager.SaveDecal();
        }
    }
}