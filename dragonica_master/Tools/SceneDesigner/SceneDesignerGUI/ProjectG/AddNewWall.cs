using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using Emergent.Gamebryo.SceneDesigner.Framework;

namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	public partial class AddNewWall : Form
	{
		public string m_strFilename = null;
		public ArrayList m_pkPaletteList = new ArrayList();
		public string m_strSelectPalette = null;

		public AddNewWall()
		{
			InitializeComponent();
			Initialize();
		}

		private void Initialize()
		{
			uint uiPalCount = MFramework.Instance.PaletteManager.PaletteCount;

			MPalette[] pkPalette = MFramework.Instance.PaletteManager.GetPalettes();

			m_cbbPalette.Items.Add("_None_");
			for (uint ui = 0; ui < uiPalCount; ui++)
			{
				string strPalName = pkPalette[ui].Name;
				if (strPalName.Equals("General"))
				{
					continue;
				}
				m_cbbPalette.Items.Add(strPalName);
			}

			if (uiPalCount > 0)
			{
				m_cbbPalette.Text = m_cbbPalette.Items[0].ToString();
			}
		}

		private void m_btnOpen_Click(object sender, EventArgs e)
		{
			SaveFileDialog dlg = new SaveFileDialog();
			string streamingDescriptions = "NIF files (*.nif)|*.nif";
			string streamingExtensions = "NIF";

			dlg.Filter = streamingDescriptions;
			if (dlg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
			{
				string fileName = dlg.FileName;
				string extension = streamingExtensions;

				m_tbFilename.AppendText(fileName);
				m_strFilename = fileName;
			}
		}

		private void m_btnOK_Click(object sender, EventArgs e)
		{
			if (m_strFilename != null)
			{
				m_strSelectPalette = null;

				if (m_cbbPalette.Text != null && m_cbbPalette.Items.Contains(m_cbbPalette.Text))
				{
					m_strSelectPalette = m_cbbPalette.SelectedItem.ToString();
				}
				else if (m_cbbPalette.Text == null)
				{
					m_strSelectPalette = null;
				}
				else
				{
					MessageBox.Show("팔레트 이름이 부적절 합니다.", "error");
					return;
				}

				this.DialogResult = System.Windows.Forms.DialogResult.OK;
				this.Close();
			}
			else
			{
				MessageBox.Show("파일 경로와 이름이 정확하지 않습니다.", "error");
				return;
			}
		}
	}
}