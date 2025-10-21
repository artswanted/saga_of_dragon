using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;

//#define PI	(3.141592653589793238462643f)

namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	public partial class RainDataDialog : Form
	{
		public string m_strFilename;
		public float m_fDotValue;
		public ArrayList m_pkIgnoreList = new ArrayList();
		public bool m_bIsBinaryMode = true;

		public RainDataDialog()
		{
			InitializeComponent();
			Initialize();
		}

		public void Initialize()
		{
			m_fDotValue = 0.9396926f;
			m_tbRadian.AppendText(m_fDotValue.ToString());
			m_lblAngle.Text = "각도: " + GetAngle(m_fDotValue).ToString() + "˚";

			m_lbIgnoreList.Items.Add("triggers");
			m_lbIgnoreList.Items.Add("paths");
			m_lbIgnoreList.Items.Add("physx");
		}

		public float GetAngle(float fDotValue)
		{
			float fRadian = (float)Math.Acos(fDotValue);
			float fAngle = 90.0f - (fRadian * 180.0f / (float)Math.PI);

			return fAngle;
		}

		private void m_btnOpen_Click(object sender, EventArgs e)
		{
			SaveFileDialog dlg = new SaveFileDialog();
			string streamingDescriptions = "Rain Data (*.rdt)|*.rdt";
			string streamingExtensions = "RDT";

			dlg.Filter = streamingDescriptions;
			if (dlg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
			{
				string fileName = dlg.FileName;
				string extension = streamingExtensions;

				m_tbFilename.AppendText(fileName);
				m_strFilename = fileName;
			}
		}

		private void m_tbRadian_TextChanged(object sender, EventArgs e)
		{
			bool bAble = float.TryParse(m_tbRadian.Text, out m_fDotValue);
			if (bAble)
			{
				m_lblAngle.Text = "각도: " + GetAngle(m_fDotValue).ToString() + "˚";
			}
		}

		private void m_btnOK_Click(object sender, EventArgs e)
		{
			if (m_strFilename != null)
			{
				bool bRadianAble = float.TryParse(m_tbRadian.Text, out m_fDotValue);
				if (!bRadianAble ||
					m_fDotValue < 0.0f ||
					m_fDotValue > 1.0f)
				{
					MessageBox.Show("내적값이 잘못 되었습니다.", "error");
					return;
				}

				m_bIsBinaryMode = m_rbBinary.Checked;
				for (int i=0 ; i<m_lbIgnoreList.Items.Count ; i++)
				{
					m_pkIgnoreList.Add(m_lbIgnoreList.Items[i]);
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

		private void m_btnIgnoreAdd_Click(object sender, EventArgs e)
		{
			int iIndex = m_lbIgnoreList.SelectedIndex;
			//if (iIndex == -1) return;

			RainDataIgnoreListDialog dlg = new RainDataIgnoreListDialog();
			if (dlg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
			{
				if (iIndex != -1)
				{
					m_lbIgnoreList.Items.Insert(iIndex, dlg.m_strNodename);
				}
				else
				{
					m_lbIgnoreList.Items.Add(dlg.m_strNodename);
				}
			}
		}

		private void m_btnIgnoreModify_Click(object sender, EventArgs e)
		{
			int iIndex = m_lbIgnoreList.SelectedIndex;
			if (iIndex == -1) return;

			RainDataIgnoreListDialog dlg = new RainDataIgnoreListDialog();
			dlg.Initialize(m_lbIgnoreList.Items[iIndex].ToString());
			if (dlg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
			{
				m_lbIgnoreList.Items[iIndex] = dlg.m_strNodename;
			}
		}

		private void m_btnIgnoreDelete_Click(object sender, EventArgs e)
		{
			int iIndex = m_lbIgnoreList.SelectedIndex;
			if (iIndex == -1) return;

			m_lbIgnoreList.Items.RemoveAt(iIndex);
		}

		private void m_lbIgnoreList_DoubleClick(object sender, EventArgs e)
		{
			int iIndex = m_lbIgnoreList.SelectedIndex;
			if (iIndex == -1) return;

			RainDataIgnoreListDialog dlg = new RainDataIgnoreListDialog();
			dlg.Initialize(m_lbIgnoreList.Items[iIndex].ToString());
			if (dlg.ShowDialog() == System.Windows.Forms.DialogResult.OK)
			{
				m_lbIgnoreList.Items[iIndex] = dlg.m_strNodename;
			}
		}
	}
}