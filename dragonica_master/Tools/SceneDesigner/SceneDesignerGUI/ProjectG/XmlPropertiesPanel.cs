using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using System.Collections;
using System.Globalization;
using System.Xml;
using System.IO;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.GUI.Utility;

namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	public partial class XmlPropertiesPanel : Form
	{
		#region Private Data
		private IUICommandService m_uiCommandService;
		private XmlManager m_kXmlManager;
		private XmlSettings m_kXmlSettings = new XmlSettings();
		#endregion

		public XmlPropertiesPanel()
		{
			InitializeComponent();

			// PropertyGrid 연결
			this.m_gridProperties.SelectedObject = m_kXmlSettings;
			LoadConfig();
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

		public void SetXmlManager(ref XmlManager pXmlManager)
		{
			m_kXmlManager = pXmlManager;
		}

		// ex) C:\\Windows\\System, ../Data/Bin/Test.exe
		// out -> C:\\Windows\\Data\\Bin\\Test.exe
		private string RelativeToAbsoluteDir(string strAbsDir, string strRelDir)
		{
			if (strAbsDir == null || strAbsDir.Length <= 0)
			{
				return strRelDir;
			}

			strAbsDir = strAbsDir.Replace('/', '\\');
			strRelDir = strRelDir.Replace('/', '\\');

			if (strRelDir.Substring(0, 2).Equals(".\\"))
			{
				strRelDir = strRelDir.Remove(0, 2);
			}
			while(true)
			{
				if (strRelDir.Substring(0, 3).Equals("..\\"))
				{
					strRelDir = strRelDir.Remove(0, 3);
					int iIdx = strAbsDir.LastIndexOf('\\');
					strAbsDir = strAbsDir.Remove(iIdx, strAbsDir.Length - iIdx);
				}
				else
				{
					break;
				}
			}

			while (strRelDir.Contains("\\"))
			{
				int iIdx = strRelDir.IndexOf('\\');
				string strFolder = strRelDir.Substring(0, iIdx);
				strRelDir = strRelDir.Remove(0, iIdx + 1);

				strAbsDir += '\\';
				strAbsDir += strFolder;
			}
			strAbsDir += '\\';
			strAbsDir += strRelDir;

			return strAbsDir;
		}

		private void LoadConfig()
		{
			if (MFramework.Instance.StreamManager.LoadConfig("config.txt"))
			{
				m_kXmlSettings.BinPath = MFramework.Instance.StreamManager.BinPath;
			}
		}

		private void SaveConfig()
		{
			MFramework.Instance.StreamManager.BinPath = m_kXmlSettings.BinPath;
			MFramework.Instance.StreamManager.SaveConfig("config.txt");
		}

		// Xml 파일이 로드 될때 속성창 정보를 업데이트 한다.
		public void PanelListLoad()
		{
			LoadConfig();

			XmlDocument pDoc = m_kXmlManager.GetDocument();
			XmlElement pRoot = m_kXmlManager.GetDocument().DocumentElement;
			XmlNodeList pNodeList = pRoot.ChildNodes;

			if (pRoot.Name.Equals("WORLD"))
				m_kXmlSettings.WorldID = pRoot.Attributes.GetNamedItem("ID").InnerText;

			for (int i = 0; i < pNodeList.Count; i++)
			{
				XmlNode pNode = pNodeList.Item(i);
				if (pNode.Name.Equals("NIFPATH"))
				{
					string strAbsDir = RelativeToAbsoluteDir(m_kXmlSettings.BinPath, pNode.InnerText);
					m_kXmlSettings.MapFilePath = strAbsDir;
				}
				else if (pNode.Name.Equals("GSAPATH"))
				{
					string strAbsDir = RelativeToAbsoluteDir(m_kXmlSettings.BinPath, pNode.InnerText);
					m_kXmlSettings.MapFilePath = strAbsDir;
				}
				else if (pNode.Name.Equals("SCRIPT"))
				{
					string strAbsDir = RelativeToAbsoluteDir(m_kXmlSettings.BinPath, pNode.InnerText);
					m_kXmlSettings.ScriptFilePath = strAbsDir;
				}
				else if (pNode.Name.Equals("MAPVALUE"))
				{
					XmlNode pAttNode = pNode.Attributes.GetNamedItem("PLAYERKILL");
					m_kXmlSettings.MapValue.PlayerKill = int.Parse(pAttNode.InnerText);
					pAttNode = pNode.Attributes.GetNamedItem("TYPE");
					m_kXmlSettings.MapValue.Type = int.Parse(pAttNode.InnerText);
				}
				else if (pNode.Name.Equals("BLOOM"))
				{
					for (int j = 0; j < pNode.Attributes.Count; j++)
					{
						XmlNode pAttNode = pNode.Attributes.Item(j);
						if (pAttNode.Name.Equals("BLUR"))
							m_kXmlSettings.Bloom.Blur = float.Parse(pAttNode.InnerText);
						else if (pAttNode.Name.Equals("BRIGHT"))
							m_kXmlSettings.Bloom.Bright = float.Parse(pAttNode.InnerText);
						else if (pAttNode.Name.Equals("SRC"))
							m_kXmlSettings.Bloom.Src = float.Parse(pAttNode.InnerText);
						else if (pAttNode.Name.Equals("GLOW"))
							m_kXmlSettings.Bloom.Glow = float.Parse(pAttNode.InnerText);
						else if (pAttNode.Name.Equals("HIGHLIGHT"))
							m_kXmlSettings.Bloom.HighLight = float.Parse(pAttNode.InnerText);
						else
							MessageBox.Show(pDoc.BaseURI + "의 BLOOM에 정확하지 않은 정보가 들어있습니다. : " + pAttNode.Name);
					}
				}
				else if (pNode.Name.Equals("BGSOUND"))
				{
					XmlNode pAttNode = pNode.Attributes.GetNamedItem("VOLUME");
					m_kXmlSettings.BGSound.Volume = float.Parse(pAttNode.InnerText);

					string strAbsDir = RelativeToAbsoluteDir(m_kXmlSettings.BinPath, pNode.InnerText);
					m_kXmlSettings.BGSound.BGSoundPath = strAbsDir;
				}
			}

			this.m_gridProperties.Refresh();
		}

		// 속성이 바뀔시에는 XMLDocument 내용을 수정한다.
		private void m_gridProperties_PropertyValueChanged(object s, PropertyValueChangedEventArgs e)
		{
			SaveConfig();

			m_kXmlManager.ActionFileList(m_kXmlSettings.BinPath + "..\\XML\\Action");

			// MapFilePath 의 상대경로 구하기.
			string strMapFilePath = "";
			if (m_kXmlSettings.BinPath.Length > 0)
			{
				if (m_kXmlSettings.MapFilePath.Length > 0)
				{
					strMapFilePath =
						SceneManagementUtilities.BuildRelativePath(
						m_kXmlSettings.BinPath,
						m_kXmlSettings.MapFilePath);
					if (strMapFilePath.Substring(0, 2).Equals(".\\"))
					{
						strMapFilePath = strMapFilePath.Remove(0, 2);
					}
					strMapFilePath = strMapFilePath.Replace("\\", "/");
				}
			}
			// ScriptFilePath 의 상대경로 구하기.
			string strScriptFilePath = "";
			if (m_kXmlSettings.BinPath.Length > 0)
			{
				if (m_kXmlSettings.ScriptFilePath.Length > 0)
				{
					strScriptFilePath =
						SceneManagementUtilities.BuildRelativePath(
						m_kXmlSettings.BinPath,
						m_kXmlSettings.ScriptFilePath);
					if (strScriptFilePath.Substring(0, 2).Equals(".\\"))
					{
						strScriptFilePath = strScriptFilePath.Remove(0, 2);
					}
					strScriptFilePath = strScriptFilePath.Replace("\\", "/");
				}
			}
			// BGSoundPath의 상대경로 구하기.
			string strBGSoundPath = "";
			if (m_kXmlSettings.BinPath.Length > 0)
			{
				if (m_kXmlSettings.BGSound.BGSoundPath.Length > 0)
				{
					strBGSoundPath =
						SceneManagementUtilities.BuildRelativePath(
						m_kXmlSettings.BinPath,
						m_kXmlSettings.BGSound.BGSoundPath);
					if (strBGSoundPath.Substring(0, 2).Equals(".\\"))
					{
						strBGSoundPath = strBGSoundPath.Remove(0, 2);
					}
					strBGSoundPath = strBGSoundPath.Replace("\\", "/");
				}
			}
			
			XmlDocument pDocument = m_kXmlManager.GetDocument();
			XmlElement pRoot = pDocument.DocumentElement;
			XmlNodeList pNodeList = pRoot.ChildNodes;
			if (pRoot.Name.Equals("WORLD") && m_kXmlSettings.WorldID != "")
				pRoot.Attributes.GetNamedItem("ID").InnerText = m_kXmlSettings.WorldID;

			for (int i = 0; i < pNodeList.Count; i++)
			{
				XmlNode pNode = pNodeList.Item(i);
				if (pNode.Name.Equals("NIFPATH"))
				{
					pNode.InnerText = strMapFilePath;// m_kXmlSettings.MapFilePath;
				}
				else if (pNode.Name.Equals("GSAPATH"))
				{
					pNode.InnerText = strMapFilePath;// m_kXmlSettings.MapFilePath;
				}
				else if (pNode.Name.Equals("SCRIPT"))
				{
					pNode.InnerText = strScriptFilePath;// m_kXmlSettings.ScriptFilePath;
				}
				else if (pNode.Name.Equals("MAPVALUE"))
				{
					XmlNode pAttNode = pNode.Attributes.GetNamedItem("PLAYERKILL");
					pAttNode.InnerText = Convert.ToString(m_kXmlSettings.MapValue.PlayerKill);
					pAttNode = pNode.Attributes.GetNamedItem("TYPE");
					pAttNode.InnerText = Convert.ToString(m_kXmlSettings.MapValue.Type);
				}
				else if (pNode.Name.Equals("BLOOM"))
				{
					for (int j = 0; j < 5; j++)
					{
						XmlNode pAttNode = pNode.Attributes.Item(j);
						if (pAttNode.Name.Equals("BLUR"))
						{
							pAttNode.InnerText = Convert.ToString(m_kXmlSettings.Bloom.Blur);
							MFramework.Instance.BrightBloom.Blur = m_kXmlSettings.Bloom.Blur;
						}
						else if (pAttNode.Name.Equals("BRIGHT"))
						{
							pAttNode.InnerText = Convert.ToString(m_kXmlSettings.Bloom.Bright);
							MFramework.Instance.BrightBloom.Brightness = m_kXmlSettings.Bloom.Bright;
						}
						else if (pAttNode.Name.Equals("SRC"))
						{
							pAttNode.InnerText = Convert.ToString(m_kXmlSettings.Bloom.Src);
							MFramework.Instance.BrightBloom.SceneIntensity = m_kXmlSettings.Bloom.Src;
						}
						else if (pAttNode.Name.Equals("GLOW"))
						{
							pAttNode.InnerText = Convert.ToString(m_kXmlSettings.Bloom.Glow);
							MFramework.Instance.BrightBloom.GlowIntensity = m_kXmlSettings.Bloom.Glow;
						}
						else if (pAttNode.Name.Equals("HIGHLIGHT"))
						{
							pAttNode.InnerText = Convert.ToString(m_kXmlSettings.Bloom.HighLight);
							MFramework.Instance.BrightBloom.HighLightIntensity = m_kXmlSettings.Bloom.HighLight;
						}
					}

					MFramework.Instance.BrightBloom.IsUpdated = true;
					if (m_kXmlSettings.Bloom.ApplyView)
						MFramework.Instance.BrightBloom.IsActive = true;
					else
						MFramework.Instance.BrightBloom.IsActive = false;
					this.m_gridProperties.Refresh();

					// MFramework.Instance.ViewportManager.ActiveViewport.UpdateBloomParams();
				}
				else if (pNode.Name.Equals("BGSOUND"))
				{
					XmlNode pAttNode = pNode.Attributes.GetNamedItem("VOLUME");
					pAttNode.InnerText = Convert.ToString(m_kXmlSettings.BGSound.Volume);
					pNode.InnerText = strBGSoundPath;// m_kXmlSettings.BGSound.BGSoundPath;
				}
			}
		}
	}

	#region PropertyGrid_XmlSettings
	public class XmlSettings
	{
		private string m_strID = "w_none";
		private string m_strBinDirPath = ""; // "D:\\Dev\\ProjectG\\SFreedom_Dev\\XML";
		private string m_strMapFilePath = ""; // "../Data/4_world/sector_a/01_t_delhapa/?.nif";
		private string m_strScriptFilePath = ""; // "../Script/World/?.lua";
		private MapValue m_kMapValue = new MapValue();
		private BloomOption m_kBloom = new BloomOption();
		private BGSoundOption m_kBGSound = new BGSoundOption();

		[EditorAttribute(typeof(System.Windows.Forms.Design.FolderNameEditor), typeof(System.Drawing.Design.UITypeEditor))]
		[CategoryAttribute("설정"),
		ReadOnlyAttribute(false),
		DefaultValueAttribute(true),
		DescriptionAttribute("실행 파일이 있는 폴더를 지정합니다.\nSFreedom_Dev\\Release 경로를 지정해 주세요.")]
		public string BinPath
		{
			get { return m_strBinDirPath; }
			set { m_strBinDirPath = value; }
		}

		[CategoryAttribute("설정"),
		ReadOnlyAttribute(false),
		DefaultValueAttribute(true),
		DescriptionAttribute("World ID를 지정합니다.")]
		public string WorldID
		{
			get { return m_strID; }
			set { m_strID = value; }
		}

		[EditorAttribute(typeof(System.Windows.Forms.Design.FileNameEditor), typeof(System.Drawing.Design.UITypeEditor))]
		[CategoryAttribute("설정"),
		ReadOnlyAttribute(false),
		DefaultValueAttribute(true),
		DescriptionAttribute("맵 파일 경로를 지정합니다.\nnif 또는 gsa 파일 경로를 지정해 줍니다.")]
		public string MapFilePath
		{
			get { return m_strMapFilePath; }
			set { m_strMapFilePath = value; }
		}

		[EditorAttribute(typeof(System.Windows.Forms.Design.FileNameEditor), typeof(System.Drawing.Design.UITypeEditor))]
		[CategoryAttribute("설정"),
		ReadOnlyAttribute(false),
		DefaultValueAttribute(true),
		DescriptionAttribute("Script 파일 경로를 지정합니다.")]
		public string ScriptFilePath
		{
			get { return m_strScriptFilePath; }
			set { m_strScriptFilePath = value; }
		}

		[CategoryAttribute("설정"),
		ReadOnlyAttribute(true),
		DescriptionAttribute("맵 상태를 지정합니다. (서버)")]
		public MapValue MapValue
		{
			get { return m_kMapValue; }
			set { m_kMapValue = value; }
		}

		[CategoryAttribute("설정"),
		ReadOnlyAttribute(true),
		DescriptionAttribute("Bloom 값을 지정합니다.")]
		public BloomOption Bloom
		{
			get { return m_kBloom; }
			set { m_kBloom = value; }
		}

		[CategoryAttribute("설정"),
		ReadOnlyAttribute(true),
		DescriptionAttribute("BGSound 값을 지정합니다.")]
		public BGSoundOption BGSound
		{
			get { return m_kBGSound; }
			set { m_kBGSound = value; }
		}
	}

	[TypeConverterAttribute(typeof(MapValueConverter))]
	public class MapValue
	{
		public int playerkill = 0;
		public int type = 1;

		[DefaultValueAttribute(true),
		DescriptionAttribute("유저간 서로 전투가 가능한 맵인가. 0=불가능, 1=가능")]
		public int PlayerKill
		{
			get { return playerkill; }
			set { playerkill = value; }
		}
		[DefaultValueAttribute(true),
	   DescriptionAttribute("월드 속성을 부여.\n일반맵 : 1, 인던맵 : 2, PVP_KTH : 32, 마을맵 : 8, 마이룸 : 16")]
		public int Type
		{
			get { return type; }
			set { type = value; }
		}
	}
	public class MapValueConverter : ExpandableObjectConverter
	{
		public override bool CanConvertTo(ITypeDescriptorContext context,
								  System.Type destinationType)
		{
			if (destinationType == typeof(MapValue))
				return true;

			return base.CanConvertTo(context, destinationType);
		}
		public override object ConvertTo(ITypeDescriptorContext context,
							   CultureInfo culture,
							   object value,
							   System.Type destinationType)
		{
			if (destinationType == typeof(System.String) &&
				 value is MapValue)
			{
				MapValue so = (MapValue)value;

				return "PlayerKill: " + so.playerkill +
					", Type :" + so.type;
			}
			return base.ConvertTo(context, culture, value, destinationType);
		}
		public override bool CanConvertFrom(ITypeDescriptorContext context,
							  System.Type sourceType)
		{
			if (sourceType == typeof(string))
				return true;

			return base.CanConvertFrom(context, sourceType);
		}
		public override object ConvertFrom(ITypeDescriptorContext context,
							  CultureInfo culture, object value)
		{
			if (value is string)
			{
				try
				{
					string s = (string)value;
					int colon = s.IndexOf(':');
					int comma = s.IndexOf(',');

					if (colon != -1 && comma != -1)
					{
						string PlayerKill = s.Substring(colon + 1,
														(comma - colon - 1));
						colon = s.IndexOf(':', comma + 1);
						comma = s.IndexOf(',', comma + 1);

						string Type = s.Substring(colon + 1);

						MapValue so = new MapValue();

						so.playerkill = int.Parse(PlayerKill);
						so.type = int.Parse(Type);

						return so;
					}
				}
				catch
				{
					throw new ArgumentException(
						"+ (string)value +를" +
										   "MapValue 형식으로" + "변환할 수 없습니다");
				}
			}
			return base.ConvertFrom(context, culture, value);
		}
	}

	[TypeConverterAttribute(typeof(BloomConverter))]
	public class BloomOption
	{
		public bool applyview = false;
		public float blur = 0.65f;
		public float bright = 8.8f;
		public float src = 0.7f;
		public float glow = 0.35f;
		public float highlight = -0.04f;

		[DefaultValueAttribute(true),
		DescriptionAttribute("화면에 Bloom효과를 뿌려 확인을 해봅니다.")]
		public bool ApplyView
		{
			get { return applyview; }
			set { applyview = value; }
		}
		[DefaultValueAttribute(true),
		DescriptionAttribute("현재 적용 불가능.")]
		public float Blur
		{
			get { return blur; }
			set { blur = value; }
		}
		[DefaultValueAttribute(true),
		DescriptionAttribute("현재 적용 불가능.")]
		public float Bright
		{
			get { return bright; }
			set { bright = value; }
		}
		[DefaultValueAttribute(true),
		DescriptionAttribute("적용 됨.")]
		public float Src
		{
			get { return src; }
			set { src = value; }
		}
		[DefaultValueAttribute(true),
		DescriptionAttribute("적용 됨.")]
		public float Glow
		{
			get { return glow; }
			set { glow = value; }
		}
		[DefaultValueAttribute(true),
		DescriptionAttribute("현재 적용 불가능.")]
		public float HighLight
		{
			get { return highlight; }
			set { highlight = value; }
		}
	}
	public class BloomConverter : ExpandableObjectConverter
	{
		public override bool CanConvertTo(ITypeDescriptorContext context,
								  System.Type destinationType)
		{
			if (destinationType == typeof(BloomOption))
				return true;

			return base.CanConvertTo(context, destinationType);
		}
		public override object ConvertTo(ITypeDescriptorContext context,
							   CultureInfo culture,
							   object value,
							   System.Type destinationType)
		{
			if (destinationType == typeof(System.String) &&
				 value is BloomOption)
			{
				BloomOption so = (BloomOption)value;

				return "Blur값: " + so.blur +
					", Bright값: " + so.bright +
					", Glow값: " + so.glow +
					", HighLight값: " + so.highlight +
					", Src값 :" + so.src;
			}
			return base.ConvertTo(context, culture, value, destinationType);
		}
		public override bool CanConvertFrom(ITypeDescriptorContext context,
							  System.Type sourceType)
		{
			if (sourceType == typeof(string))
				return true;

			return base.CanConvertFrom(context, sourceType);
		}
		public override object ConvertFrom(ITypeDescriptorContext context,
							  CultureInfo culture, object value)
		{
			if (value is string)
			{
				try
				{
					string s = (string)value;
					int colon = s.IndexOf(':');
					int comma = s.IndexOf(',');

					if (colon != -1 && comma != -1)
					{
						string Blur = s.Substring(colon + 1,
														(comma - colon - 1));
						colon = s.IndexOf(':', comma + 1);
						comma = s.IndexOf(',', comma + 1);

						string Bright = s.Substring(colon + 1,
								(comma - colon - 1));
						colon = s.IndexOf(':', comma + 1);
						comma = s.IndexOf(',', comma + 1);

						string Src = s.Substring(colon + 1,
								(comma - colon - 1));
						colon = s.IndexOf(':', comma + 1);
						comma = s.IndexOf(',', comma + 1);

						string Glow = s.Substring(colon + 1,
														(comma - colon - 1));
						colon = s.IndexOf(':', comma + 1);

						string HighLight = s.Substring(colon + 1);

						BloomOption so = new BloomOption();

						so.blur = float.Parse(Blur);
						so.bright = float.Parse(Bright);
						so.src = float.Parse(Src);
						so.glow = float.Parse(Glow);
						so.highlight = float.Parse(HighLight);

						return so;
					}
				}
				catch
				{
					throw new ArgumentException(
						"+ (string)value +를" +
										   "BloomOption 형식으로" + "변환할 수 없습니다");
				}
			}
			return base.ConvertFrom(context, culture, value);
		}
	}

	[TypeConverterAttribute(typeof(BGSoundConverter))]
	public class BGSoundOption
	{
		public string bgsoundpath = ""; // "../sound/?.mp3";
		public float volume = 1;

		[EditorAttribute(typeof(System.Windows.Forms.Design.FileNameEditor), typeof(System.Drawing.Design.UITypeEditor))]
		[DefaultValueAttribute(true)]
		public string BGSoundPath
		{
			get { return bgsoundpath; }
			set { bgsoundpath = value; }
		}
		[DefaultValueAttribute(true),
		DescriptionAttribute("사운드 볼륨을 설정 합니다. (0~100)")]
		public float Volume
		{
			get { return volume; }
			set { volume = value; }
		}
	}
	public class BGSoundConverter : ExpandableObjectConverter
	{
		public override bool CanConvertTo(ITypeDescriptorContext context,
								  System.Type destinationType)
		{
			if (destinationType == typeof(BGSoundOption))
				return true;

			return base.CanConvertTo(context, destinationType);
		}
		public override object ConvertTo(ITypeDescriptorContext context,
							   CultureInfo culture,
							   object value,
							   System.Type destinationType)
		{
			if (destinationType == typeof(System.String) &&
				 value is BGSoundOption)
			{
				BGSoundOption so = (BGSoundOption)value;

				return "Path: " + so.bgsoundpath +
					", Volume: " + so.volume;
			}
			return base.ConvertTo(context, culture, value, destinationType);
		}
		public override bool CanConvertFrom(ITypeDescriptorContext context,
							  System.Type sourceType)
		{
			if (sourceType == typeof(string))
				return true;

			return base.CanConvertFrom(context, sourceType);
		}
		public override object ConvertFrom(ITypeDescriptorContext context,
							  CultureInfo culture, object value)
		{
			if (value is string)
			{
				try
				{
					string s = (string)value;
					int colon = s.IndexOf(':');
					int comma = s.IndexOf(',');

					if (colon != -1 && comma != -1)
					{
						string BGSoundPath = s.Substring(colon + 1,
														(comma - colon - 1));
						colon = s.IndexOf(':', comma + 1);

						string Volume = s.Substring(colon + 1);

						BGSoundOption so = new BGSoundOption();
						so.bgsoundpath = BGSoundPath;
						so.volume = float.Parse(Volume);

						return so;
					}
				}
				catch
				{
					throw new ArgumentException(
						"+ (string)value +를" +
										   "BloomOption 형식으로" + "변환할 수 없습니다");
				}
			}
			return base.ConvertFrom(context, culture, value);
		}
	}
	#endregion
}