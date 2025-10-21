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
using System.Drawing;
using System.Windows.Forms;
using System.Runtime.InteropServices;
using System.Collections;
using System.Xml;
using NiManagedToolInterface;

namespace AnimationToolUI
{
	class MacroData
	{
		//Keys m_kKey;
		string m_kKey;
		string m_kMacroName;
		MSequence.MTextKey m_kEventInfo;

		public MacroData(string kKey/*Keys kKey*/, string kMacroName, MSequence.MTextKey kEventInfo)
		{
			m_kKey = kKey;
			m_kMacroName = kMacroName;
			m_kEventInfo = kEventInfo;
		}

		public string Key
		{
			get { return m_kKey; }
			set { m_kKey = value; }
		}

		public string MacroName
		{
			get { return m_kMacroName; }
			set { m_kMacroName = value; }
		}

		public MSequence.MTextKey EventInfo
		{
			get { return m_kEventInfo; }
			set { m_kEventInfo = value; }
		}
	}

    class MacroManager
	{
#if MacroUseInputManager
		#region Delegates
		private AppEvents.EventHandler_SequenceView m_kSequenceView = null;
	
		private void RegisterDelegates()
		{
		    UnregisterDelegates();

		    // Create delegates.
		    m_kSequenceView = new AppEvents.EventHandler_SequenceView(OnSequenceView);

		    // Register delegates.
		    AppEvents.SequenceView += m_kSequenceView;
		}

		private void UnregisterDelegates()
		{
		    if (m_kSequenceView != null)
		        AppEvents.SequenceView -= m_kSequenceView;
		}

		#endregion
#endif 

		static private MacroManager ms_kThis = null;
		Hashtable m_kMacroTable = new Hashtable();
		MacroForm m_kMacroForm = new MacroForm();

		private MacroManager()
		{
			ms_kThis = this;
			ReadFromXml("c:\\macro.xml");
		}

		static public void Create()
		{
			if(ms_kThis != null)
			{
				return;
			}

			new MacroManager();
#if MacroUseInputManager
			ms_kThis.RegisterDelegates();
#endif
		}

		static public MacroManager Instance
		{
			get 
			{
				if (ms_kThis == null)
				{
					new MacroManager();
				}

				return ms_kThis;
			}
		}

		public void OpenMacroForm()
		{
			m_kMacroForm.ShowDialog();
		}

		public Hashtable GetMacroTable()
		{
			return m_kMacroTable;
		}

		public void OpenMacroRegisterDialog(MSequence.MTextKey kTextKeyInfo)
		{
			RegisterMacroForm kRegisterForm = new RegisterMacroForm();
			kRegisterForm.SetEventInfo(ref kTextKeyInfo);
			if (kRegisterForm.ShowDialog() != System.Windows.Forms.DialogResult.OK)
			{
				return;
			}

			if (AddMacro(kRegisterForm.MacroKey.ToString(), kRegisterForm.MacroName, kTextKeyInfo))
			{
				m_kMacroForm.ShowDialog();
				WriteToXml("c:\\macro.xml");
			}
		}

		public bool AddMacro(string kKey/*Keys kKey*/, string kMacroName, MSequence.MTextKey kEventInfo)
		{
			if (m_kMacroTable.Contains(kKey) == false)
			{
				MacroData kMacroData = new MacroData(kKey, kMacroName, kEventInfo);
				m_kMacroTable.Add(kKey.ToString(), kMacroData);
				InvalidateListView();
				return true;
			}

			return false;			
		}

		public void RemoveMacro(Keys kKey)
		{

		}

		public void ExecuteMacro(Keys kKey)
		{
			MacroData kMacroData = (MacroData)m_kMacroTable[kKey.ToString()];
			if (kMacroData.EventInfo.ExtraData is MSequence.MTextKey.MAudioData)
			{
				MSequence.MTextKey.MAudioData kAudioData = (MSequence.MTextKey.MAudioData)kMacroData.EventInfo.ExtraData;
				MFramework.Instance.Animation.AddSoundEvent(
					kMacroData.EventInfo.Text, 
					kMacroData.EventInfo.Time, 
					kAudioData.SoundName,
					kAudioData.Volume,
					kAudioData.MinDistance,
					kAudioData.MaxDistance);
			}
			else if (kMacroData.EventInfo.ExtraData is MSequence.MTextKey.MEffectData)
			{
				MSequence.MTextKey.MEffectData kEffectData = (MSequence.MTextKey.MEffectData)kMacroData.EventInfo.ExtraData;
				MFramework.Instance.Animation.AddEffectEvent(
					kMacroData.EventInfo.Text,
					kMacroData.EventInfo.Time,
					kEffectData.EffectName,
					kEffectData.AttachPoint,
					kEffectData.Scale,
					kEffectData.Attach,
					kEffectData.Loop);
			}
			else
			{
				MFramework.Instance.Animation.AddTextKeyEvent(
					kMacroData.EventInfo.Text,
					kMacroData.EventInfo.Time);
			}

			AppEvents.RaiseTimeSliderChanged();
		}

#if MacroUseInputManager
		public void OnSequenceView(string kKey, string kModifier)
		{
			
		}
#endif
		public bool IsAlreadyBind(Keys kKey)
		{
			return m_kMacroTable.Contains(kKey);
		}

		public void ReadFromXml(string kXmlPath)
		{
			// 지정한 Xml파일로 부터 Macro List를 읽어 온다.
			XmlReaderSettings kSettings = new XmlReaderSettings();

			kSettings.ConformanceLevel = ConformanceLevel.Fragment;
			kSettings.IgnoreWhitespace = true;
			kSettings.IgnoreComments = true;

			XmlReader kReader = null;

			try
			{
				kReader = XmlReader.Create(kXmlPath, kSettings);
				if (kReader == null)
				{
					return;
				}
			}
			catch (Exception e)
			{
				return;
			}

			// 최상위 노드를 찾는다.
			while(kReader.Read())
			{
				if(kReader.NodeType == XmlNodeType.Element)
				{
					// 최상위 노드가 <ITEM>이 아니면 종료
					if(kReader.Name.CompareTo("MACRO") != 0)
					{
						kReader.Close();
						return;
					}
					else
					{
						break;
					}
				}
			}

			
			while(kReader.Read())
			{
				if(kReader.NodeType == XmlNodeType.Element)
				{
					string kTagName = kReader.Name;
					if(kTagName.CompareTo("ITEM") == 0)
					{
						string kKeyData = "";
						string kMacroName = "";
						string kEventKey = "";
						string kType = "";
						float fEventTime = 0.0f;
						string kEventSrc = "";
						float fVol = 0.0f;
						float fMax = 0.0f;
						float fMin = 0.0f;
						float fScale = 0.0f;
						string kAttachPoint = "";

						while(kReader.MoveToNextAttribute())
						{		
							kTagName = kReader.Name;
							if (kTagName.CompareTo("KEY") == 0)
							{
								kKeyData = kReader.Value;
							}
							else if (kTagName.CompareTo("NAME") == 0)
							{
								kMacroName = kReader.Value;
							}
							else if (kTagName.CompareTo("EVENT_KEY") == 0)
							{
								kEventKey = kReader.Value;
							}
							else if (kTagName.CompareTo("EVENT_TIME") == 0)
							{
								fEventTime = float.Parse(kReader.Value);
							}
							else if (kTagName.CompareTo("EVENT_SRC") == 0)
							{
								kEventSrc = kReader.Value;
							}
							else if (kTagName.CompareTo("TYPE") == 0)
							{
								kType = kReader.Value;
							}
							else if (kTagName.CompareTo("VOL") == 0)
							{
								fVol = float.Parse(kReader.Value);
							}
							else if (kTagName.CompareTo("MIN") == 0)
							{
								fMin = float.Parse(kReader.Value);
							}
							else if (kTagName.CompareTo("MAX") == 0)
							{
								fMax = float.Parse(kReader.Value);
							}
							else if (kTagName.CompareTo("ATTACH_POINT") == 0)
							{
								kAttachPoint = kReader.Value;
							}
							else if (kTagName.CompareTo("SCALE") == 0)
							{
								fScale = float.Parse(kReader.Value);
							}
						}

						MSequence.MTextKey kEventInfo = new MSequence.MTextKey(fEventTime, kEventKey);
						if (kType == "SOUND")
						{
							kEventInfo.ExtraData = new MSequence.MTextKey.MAudioData(kEventSrc, fVol, fMin, fMax);

						}
						else if (kType == "EFFECT")
						{
							kEventInfo.ExtraData = new MSequence.MTextKey.MEffectData(kEventSrc, kAttachPoint, fScale, true, false);
						}

						AddMacro(kKeyData, kMacroName, kEventInfo);
					}
				}
			}

			kReader.Close();
		}

		public void WriteToXml(string kXmlPath)
		{
			// 지정한 Xml파일로 Macro List를 저장한다.
			XmlTextWriter kWriter = new XmlTextWriter(kXmlPath, System.Text.Encoding.UTF8);

			// <ITEM NAME = "" WEAPON = "" FILTER = "">
			kWriter.WriteStartElement("MACRO");
			kWriter.WriteWhitespace("\n\t");

			foreach (DictionaryEntry e in m_kMacroTable)
			{
				MacroData kData = (MacroData)e.Value;

				kWriter.WriteStartElement("ITEM");
				kWriter.WriteAttributeString("KEY", kData.Key.ToString());
				kWriter.WriteAttributeString("NAME", kData.MacroName);
				kWriter.WriteAttributeString("EVENT_KEY", kData.EventInfo.Text);
				kWriter.WriteAttributeString("EVENT_TIME", kData.EventInfo.Time.ToString("f2"));

				if (kData.EventInfo.ExtraData is MSequence.MTextKey.MAudioData)
				{
					MSequence.MTextKey.MAudioData kAudioData = 
						(MSequence.MTextKey.MAudioData) kData.EventInfo.ExtraData;
					kWriter.WriteAttributeString("TYPE", "SOUND");
					kWriter.WriteAttributeString("EVENT_SRC", kAudioData.SoundName);
					kWriter.WriteAttributeString("VOL", kAudioData.Volume.ToString("f1"));
					kWriter.WriteAttributeString("MIN", kAudioData.MinDistance.ToString("f1"));
					kWriter.WriteAttributeString("MAX", kAudioData.MaxDistance.ToString("f1"));
				}
				else if (kData.EventInfo.ExtraData is MSequence.MTextKey.MEffectData)
				{
					MSequence.MTextKey.MEffectData kEffectData =
						(MSequence.MTextKey.MEffectData)kData.EventInfo.ExtraData;
					kWriter.WriteAttributeString("TYPE", "EFFECT");
					kWriter.WriteAttributeString("EVENT_SRC", kEffectData.EffectName);
					kWriter.WriteAttributeString("ATTACHPOINT", kEffectData.AttachPoint);
					kWriter.WriteAttributeString("SCALE", kEffectData.Scale.ToString("f1"));
				}
				
				kWriter.WriteEndElement();
				kWriter.WriteWhitespace("\n\t");
			}
			
			kWriter.WriteWhitespace("\n");
			kWriter.WriteFullEndElement();

			kWriter.Close();
		}

		public void InvalidateListView()
		{
			m_kMacroForm.UpdateListView(m_kMacroTable);
		}

		public bool OnKeyDown(Keys key)
		{
			if (!m_kMacroTable.Contains(key.ToString()))
			{
				return false;
			}

			ExecuteMacro(key);
			return true;
		}
    }
}
