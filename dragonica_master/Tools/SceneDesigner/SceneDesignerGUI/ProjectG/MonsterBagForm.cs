using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;

namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	public partial class MonsterBagForm : Form
	{
		public ArrayList m_pmMonsterList = new ArrayList();
		public int[] m_aiMonsterID = new int[10];
		public int[] m_aiMonsterRate = new int[10];
		public int m_iBagID;
		public String m_strBagMemo;

		public MonsterBagForm()
		{
			InitializeComponent();
		}

		public void DataUpdate()
		{
			m_tbBagID.Text = m_iBagID.ToString();
			m_tbMemo.Text = m_strBagMemo;

			m_tbMon1ID.Text = m_aiMonsterID[0].ToString();
			m_tbMon1Rate.Text = m_aiMonsterRate[0].ToString();
			m_tbMon2ID.Text = m_aiMonsterID[1].ToString();
			m_tbMon2Rate.Text = m_aiMonsterRate[1].ToString();
			m_tbMon3ID.Text = m_aiMonsterID[2].ToString();
			m_tbMon3Rate.Text = m_aiMonsterRate[2].ToString();
			m_tbMon4ID.Text = m_aiMonsterID[3].ToString();
			m_tbMon4Rate.Text = m_aiMonsterRate[3].ToString();
			m_tbMon5ID.Text = m_aiMonsterID[4].ToString();
			m_tbMon5Rate.Text = m_aiMonsterRate[4].ToString();
			m_tbMon6ID.Text = m_aiMonsterID[5].ToString();
			m_tbMon6Rate.Text = m_aiMonsterRate[5].ToString();
			m_tbMon7ID.Text = m_aiMonsterID[6].ToString();
			m_tbMon7Rate.Text = m_aiMonsterRate[6].ToString();
			m_tbMon8ID.Text = m_aiMonsterID[7].ToString();
			m_tbMon8Rate.Text = m_aiMonsterRate[7].ToString();
			m_tbMon9ID.Text = m_aiMonsterID[8].ToString();
			m_tbMon9Rate.Text = m_aiMonsterRate[8].ToString();
			m_tbMon10ID.Text = m_aiMonsterID[9].ToString();
			m_tbMon10Rate.Text = m_aiMonsterRate[9].ToString();
		}

		public void Initialize()
		{
			for (int i=0 ; i<m_pmMonsterList.Count ; i++)
			{
				MonsterData kData = (MonsterData)m_pmMonsterList[i];
				String strData = kData.m_iMonParentBagNo.ToString() + " " + kData.m_strMemo;
				m_lbMonsterList.Items.Add(strData);
			}
		}

		private void m_btnOK_Click(object sender, EventArgs e)
		{
			int.TryParse(m_tbBagID.Text, out m_iBagID);
			m_strBagMemo = m_tbMemo.Text;
			int.TryParse(m_tbMon1ID.Text, out m_aiMonsterID[0]);
			int.TryParse(m_tbMon1Rate.Text, out m_aiMonsterRate[0]);
			int.TryParse(m_tbMon2ID.Text, out m_aiMonsterID[1]);
			int.TryParse(m_tbMon2Rate.Text, out m_aiMonsterRate[1]);
			int.TryParse(m_tbMon3ID.Text, out m_aiMonsterID[2]);
			int.TryParse(m_tbMon3Rate.Text, out m_aiMonsterRate[2]);
			int.TryParse(m_tbMon4ID.Text, out m_aiMonsterID[3]);
			int.TryParse(m_tbMon4Rate.Text, out m_aiMonsterRate[3]);
			int.TryParse(m_tbMon5ID.Text, out m_aiMonsterID[4]);
			int.TryParse(m_tbMon5Rate.Text, out m_aiMonsterRate[4]);
			int.TryParse(m_tbMon6ID.Text, out m_aiMonsterID[5]);
			int.TryParse(m_tbMon6Rate.Text, out m_aiMonsterRate[5]);
			int.TryParse(m_tbMon7ID.Text, out m_aiMonsterID[6]);
			int.TryParse(m_tbMon7Rate.Text, out m_aiMonsterRate[6]);
			int.TryParse(m_tbMon8ID.Text, out m_aiMonsterID[7]);
			int.TryParse(m_tbMon8Rate.Text, out m_aiMonsterRate[7]);
			int.TryParse(m_tbMon9ID.Text, out m_aiMonsterID[8]);
			int.TryParse(m_tbMon9Rate.Text, out m_aiMonsterRate[8]);
			int.TryParse(m_tbMon10ID.Text, out m_aiMonsterID[9]);
			int.TryParse(m_tbMon10Rate.Text, out m_aiMonsterRate[9]);

			this.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.Close();
		}
	}
}