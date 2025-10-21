using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Xml;
using System.Windows.Forms;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;
using Emergent.Gamebryo.SceneDesigner.Framework;

namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	public partial class FogPanel : Form
	{
		private XmlManager m_XmlManager;

		public FogPanel()
		{
			InitializeComponent();

			m_rbRangeRQ.Checked = true;
			m_rbZLinear.Checked = false;
			m_XmlManager = null;
		}

		public void SetXmlManager(ref XmlManager kXmlMan)
		{
			m_XmlManager = kXmlMan;
		}

		public bool LoadXML()
		{
			if (null != m_XmlManager)
			{//xmlmanager가 NULL이 아니면
				//xml정보를 가져옴.
				XmlDocument kXmlDoc = m_XmlManager.GetDocument();
				//포그 태그를 찾는다.
				XmlNodeList kXmlNode = kXmlDoc.GetElementsByTagName("FOG");
				
				if (0 < kXmlNode.Count)
				{//포그 태그가 널이 아니면 포그 정보가 있는 맵임

					//모든 노드에 대해...
					//<FOG FUNC="RANGE_SQ" DENSITY="0.050000" START="75.000000" END="2180.000000" COLOR="0.000000,0.067000,0.094000"/>
					//위와 같은 포그 정보 파싱
					foreach (XmlNode kNode in kXmlNode)
					{
						if (0 < kNode.Attributes.Count)
						{
							string kFunc = (null == kNode.Attributes.GetNamedItem("FUNC") ? "" : kNode.Attributes.GetNamedItem("FUNC").InnerText);
							string kDensity = (null == kNode.Attributes.GetNamedItem("DENSITY") ? "" : kNode.Attributes.GetNamedItem("DENSITY").InnerText);
							string kStart = (null == kNode.Attributes.GetNamedItem("START")? "" : kNode.Attributes.GetNamedItem("START").InnerText);
							string kEnd = (null == kNode.Attributes.GetNamedItem("END") ? "" : kNode.Attributes.GetNamedItem("END").InnerText);
							string kColor = (null == kNode.Attributes.GetNamedItem("COLOR") ? "" : kNode.Attributes.GetNamedItem("COLOR").InnerText);

							if ("" != kFunc)
							{
								if ("Z_LINEAR" == kFunc)
								{
									m_rbZLinear.Checked = true;
								}
								else if ("RANGE_SQ" == kFunc)
								{
									m_rbRangeRQ.Checked = true;
								}
							}
							else
							{
								m_rbZLinear.Checked = true;
							}

							if ("" == kColor)
							{
								return false;
							}
							//컬러 RGB로 분해
							string kColorR = kColor.Substring(0, kColor.IndexOf(","));
							kColor = kColor.Remove(0, kColor.IndexOf(",") + 1);
							string kColorG = kColor.Substring(0, kColor.IndexOf(","));
							kColor = kColor.Remove(0, kColor.IndexOf(",") + 1);
							string kColorB = kColor;

							float fTemp;
							//근단면
							float.TryParse(kStart, out fTemp);
							m_trbNear.Value = (int)fTemp;
							//원단면
							float.TryParse(kEnd, out fTemp);
							m_trbFar.Value = (int)fTemp;
							SetWorldBound();
							MFramework.Instance.Fog.SetFogPlane((float)m_trbNear.Value, (float)m_trbFar.Value);
							//강도
							float.TryParse(kDensity, out fTemp);
							m_trbDensity.Value = (int)(fTemp * 100.0f);
							//컬러
							float fR = 0.0f, fG = 0.0f, fB = 0.0f;
							float.TryParse(kColorR, out fR);
							float.TryParse(kColorG, out fG);
							float.TryParse(kColorB, out fB);

							m_btnFogColor.BackColor = Color.FromArgb(
								1, (int)(fR * 255.0f),
								(int)(fG * 255.0f), (int)(fB * 255.0f)
								);

							MFramework.Instance.Fog.SetFogColor(
								((float)m_btnFogColor.BackColor.R) / 255.0f,
								((float)m_btnFogColor.BackColor.G) / 255.0f,
								((float)m_btnFogColor.BackColor.B) / 255.0f);
						}
					}
					//포그를 켠다
					m_cbFogOnOff.Checked = true;
				}
				else
				{
					m_cbFogOnOff.Checked = false;
				}
			}
			return true;
		}

		public void Clear()
		{//패널 초기화
			//m_XmlManager = null;
			m_trbDensity.Value = 0;
			m_trbNear.Value = 0;
			m_trbFar.Value = 0;
			m_btnFogColor.BackColor = Color.Black;
			m_cbFogOnOff.Checked = false;
			m_tbResult.Text = "";
		}

		public void SaveXML()
		{
			if (null == m_XmlManager)
			{
				return;
			}
			XmlDocument kXmlDoc = m_XmlManager.GetDocument();
			XmlNodeList kXmlNode = kXmlDoc.GetElementsByTagName("FOG");
						
			if (false == m_cbFogOnOff.Checked)
			{//포그 체크가 안되어 있으면
				if (0 < kXmlNode.Count)
				{//FOG노드가 존재하면
					//포그 노드 찾아서 삭제하고 함수 종료
					//kXmlDoc.Attributes.RemoveNamedItem("FOG");
					foreach (XmlNode kWolrdNode in kXmlDoc.ChildNodes)
					{
						if ("WORLD" == kWolrdNode.Name)
						{
							foreach (XmlNode kFogNode in kWolrdNode.ChildNodes)
							{
								if ("FOG" == kFogNode.Name)
								{
									kWolrdNode.RemoveChild(kFogNode);
								}
							}
						}
					}
					return;					
				}
			}
			else
			{//포그 체크가 되어 있으면
				float kColorR, kColorG, kColorB;
				kColorR = (float)m_btnFogColor.BackColor.R / 255.0f;
				kColorG = (float)m_btnFogColor.BackColor.G / 255.0f;
				kColorB = (float)m_btnFogColor.BackColor.B / 255.0f;
				String kColor;
				kColor = kColorR.ToString() + "," + kColorG.ToString() + "," + kColorB.ToString();

				if (0 < kXmlNode.Count)
				{//Fog노드가 존재하면
					foreach (XmlNode kNode in kXmlNode)
					{
						if (null != kNode.Attributes.GetNamedItem("DENSITY"))
						{
							kNode.Attributes.GetNamedItem("DENSITY").InnerText = ((float)m_trbDensity.Value / 100.0f).ToString();
						}
						else
						{
							XmlAttribute kDensityAttrib = kXmlDoc.CreateAttribute("DENSITY");
							kDensityAttrib.InnerText = ((float)m_trbDensity.Value / 100.0f).ToString();
							kNode.Attributes.InsertAfter(kDensityAttrib, null);
						}

						if (null != kNode.Attributes.GetNamedItem("FUNC"))
						{
							kNode.Attributes.GetNamedItem("FUNC").InnerText = (true == m_rbZLinear.Checked ? m_rbZLinear.Text : m_rbRangeRQ.Text);
						}
						else
						{
							XmlAttribute kFUNCAttrib = kXmlDoc.CreateAttribute("FUNC");
							kFUNCAttrib.InnerText = (true == m_rbZLinear.Checked ? m_rbZLinear.Text : m_rbRangeRQ.Text);
							kNode.Attributes.InsertAfter(kFUNCAttrib, null);
						}

						if (null != kNode.Attributes.GetNamedItem("START"))
						{
							kNode.Attributes.GetNamedItem("START").InnerText = ((float)m_trbNear.Value).ToString();
						}
						else
						{
							XmlAttribute kSTARTAttrib = kXmlDoc.CreateAttribute("START");
							kSTARTAttrib.InnerText = ((float)m_trbNear.Value).ToString();
							kNode.Attributes.InsertAfter(kSTARTAttrib, null);
						}

						if (null != kNode.Attributes.GetNamedItem("END"))
						{
							kNode.Attributes.GetNamedItem("END").InnerText = ((float)m_trbFar.Value).ToString();
						}
						else
						{
							XmlAttribute kENDAttrib = kXmlDoc.CreateAttribute("END");
							kENDAttrib.InnerText = ((float)m_trbFar.Value).ToString();
							kNode.Attributes.InsertAfter(kENDAttrib,null);
						}

						if (null != kNode.Attributes.GetNamedItem("COLOR"))
						{
							kNode.Attributes.GetNamedItem("COLOR").InnerText = kColor;
						}
						else
						{
							XmlAttribute kCOLORAttrib = kXmlDoc.CreateAttribute("COLOR");
							kCOLORAttrib.InnerText = kColor;
							kNode.Attributes.InsertAfter(kCOLORAttrib, null);
						}
					}
				}
				else
				{//Fog노드가 존재하지 않으면
					//포그 태그를 추가해준다.
					XmlElement kFogElement =  kXmlDoc.CreateElement("FOG");
					kFogElement.SetAttribute("DENSITY", ((float)m_trbDensity.Value / 100.0f).ToString());
					kFogElement.SetAttribute("FUNC", (true == m_rbZLinear.Checked ? m_rbZLinear.Text : m_rbRangeRQ.Text));
					kFogElement.SetAttribute("START", ((float)m_trbNear.Value).ToString());
					kFogElement.SetAttribute("END", ((float)m_trbFar.Value).ToString());
					kFogElement.SetAttribute("COLOR", kColor);

					foreach (XmlNode kNodeWorld in kXmlDoc.ChildNodes)
					{
						if ("WORLD" == kNodeWorld.Name)
						{
							kNodeWorld.InsertBefore(kFogElement, null);
						}
					}
				}
			}
		}

		private void UpdateResultString()
		{
			float kColorR, kColorG, kColorB;
			kColorR = (float)m_btnFogColor.BackColor.R / 255.0f;
			kColorG = (float)m_btnFogColor.BackColor.G / 255.0f;
			kColorB = (float)m_btnFogColor.BackColor.B / 255.0f;
			String kColor, kFogType;
			kColor = kColorR.ToString() + "," + kColorG.ToString() + "," + kColorB.ToString();

			switch (MFramework.Instance.Fog.GetFogType())
			{
				case MPgFog.eFOG_FUNC.FF_ZLINEAR:
					{
						kFogType = "Z_LINEAR";
					}
					break;
				case MPgFog.eFOG_FUNC.FF_RANGESQ:
					{
						kFogType = "RANGE_SQ";
					}
					break;
				default:
					{
						kFogType = "Z_LINEAR";
					}
					break;
			};

			String kResult = "<FOG DENSITY=" + "\"" + ((float)(m_trbDensity.Value)/100.0f).ToString() +
				"\" FUNC=\"" + kFogType +
				"\" START=\"" + ((float)(m_trbNear.Value)).ToString() +
				"\" END=\"" + ((float)(m_trbFar.Value)).ToString() +
				"\" COLOR=\"" + kColor + "\"/>";

			m_tbResult.Text = kResult;
		}

		private void m_btnFogColor_Click(object sender, EventArgs e)
		{
			ColorDialog kColorDialog = new ColorDialog();
			if (System.Windows.Forms.DialogResult.OK == kColorDialog.ShowDialog())
			{
				m_btnFogColor.BackColor = kColorDialog.Color;
				MFramework.Instance.Fog.SetFogColor(
					((float)kColorDialog.Color.R) / 255.0f,
					((float)kColorDialog.Color.G) / 255.0f,
					((float)kColorDialog.Color.B) / 255.0f);
			}
			UpdateResultString();
		}

		private void m_trbNear_MouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			if (m_trbNear.Value > m_trbFar.Value)
			{
				m_trbFar.Value = m_trbNear.Value;
			}

			MFramework.Instance.Fog.SetFogPlane((float)m_trbNear.Value, (float)m_trbFar.Value);
			UpdateResultString();
		}

		private void m_trbFar_MouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			if (m_trbNear.Value > m_trbFar.Value)
			{
				m_trbNear.Value = m_trbFar.Value;
			}
			MFramework.Instance.Fog.SetFogPlane((float)m_trbNear.Value, (float)m_trbFar.Value);
			UpdateResultString();
		}

		private void m_trbDensity_MouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			MFramework.Instance.Fog.SetFogDensity((float)m_trbDensity.Value / 100.0f);
			UpdateResultString();
		}

		private void m_cbFogOnOff_CheckedChanged(object sender, EventArgs e)
		{
			MFramework.Instance.Fog.SetFogEnable(m_cbFogOnOff.Checked);
			UpdateResultString();
		}

		public void SetWorldBound()
		{
			int iWorldSize = (int)MFramework.Instance.Fog.GetBound();
			m_trbNear.Maximum = iWorldSize;
			m_trbNear.TickFrequency = iWorldSize / 20;
			m_trbFar.Maximum = iWorldSize;
			m_trbFar.TickFrequency = iWorldSize / 20;
		}

		private void m_rbZLinear_CheckedChanged(object sender, EventArgs e)
		{
			MFramework.Instance.Fog.SetFogType(MPgFog.eFOG_FUNC.FF_ZLINEAR);
			UpdateResultString();
		}

		private void m_rbRangeRQ_CheckedChanged(object sender, EventArgs e)
		{
			MFramework.Instance.Fog.SetFogType(MPgFog.eFOG_FUNC.FF_RANGESQ);
			UpdateResultString();
		}

		private void m_btnApplyFog_Click(object sender, EventArgs e)
		{
			MFramework.Instance.Fog.UpdateFog();
		}
	}
}