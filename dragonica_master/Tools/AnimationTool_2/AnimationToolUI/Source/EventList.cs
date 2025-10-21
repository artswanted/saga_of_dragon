using System;
using System.Collections;
using System.Xml;
using System.Diagnostics;
using System.Collections.Generic;
using System.Text;

using NiManagedToolInterface;

namespace AnimationToolUI
{
	class EventList
	{
		static public bool LookUpEventListPath(string kType, ref string EventListPath)
		{
			string kRegKey = kType + "EventObjectPath";
			bool bLookUpEventList = false;
			if (!AppUtils.RestoreRegKey(kRegKey, ref EventListPath) ||
				!System.IO.File.Exists(EventListPath))
			{
				bLookUpEventList = true;
			}

			if (bLookUpEventList)
			{
				System.Windows.Forms.OpenFileDialog ofdSelectEventList = new System.Windows.Forms.OpenFileDialog();
				ofdSelectEventList.DefaultExt = "xml";
				ofdSelectEventList.Filter = "Xml file (*.xml)|*.xml";
				ofdSelectEventList.RestoreDirectory = true;
				ofdSelectEventList.Title = "Select Event Path Xml..(" + kType + ")";

				if (ofdSelectEventList.ShowDialog() == System.Windows.Forms.DialogResult.OK)
				{
					EventListPath = ofdSelectEventList.FileName;
					if (System.IO.File.Exists(EventListPath))
					{
						AppUtils.StoreRegKey(kRegKey, EventListPath);
					}
				}
				else
				{
					return false;
				}
			}
			return true;
		}

		static public bool LookUpEventList(string kType)
		{
			string EventListPath = null;
			// Event List Xml이 있는 패스를 구한다.
			if (!LookUpEventListPath(kType, ref EventListPath))
			{
				return false;
			}

			if (kType == "Effect")
			{
				// Path를 구했으면, Xml파일을 파싱한다.
				if (!MFramework.Instance.EffectMan.AddEventItems(EventListPath, kType))
				{
					return false;
				}
			}
			else if (kType == "Sound")
			{
				// Path를 구했으면, Xml파일을 파싱한다.
				if (!MFramework.Instance.AudioMan.AddEventItems(EventListPath, kType))
				{
					return false;
				}
			}

			return true;
		}

		//static private Hashtable m_mapParsingLog = new Hashtable();
		//static private Hashtable m_mapEventList = new Hashtable();

		//static public bool ParseEventList(string kEventListPath)
		//{
		//    DateTime kLastWriteTime = System.IO.File.GetLastWriteTime(kEventListPath);
		//    if (m_mapParsingLog.Contains(kEventListPath))
		//    {
		//        DateTime kLoggedTime = (DateTime)m_mapParsingLog[kEventListPath];
		//        if (kLoggedTime != null)
		//        {
		//            if (kLastWriteTime <= kLoggedTime)
		//            {
		//                return true;
		//            }
		//            m_mapParsingLog[kEventListPath] = kLastWriteTime;
		//        }
		//    }
		//    else
		//    {
		//        m_mapParsingLog.Add(kEventListPath, kLastWriteTime);
		//    }

		//    XmlTextReader kReader = new XmlTextReader(kEventListPath);
		//    if (kReader == null)
		//    {
		//        return false;
		//    }

		//    kReader.WhitespaceHandling = WhitespaceHandling.None;

		//    // 최상위 노드를 찾는다.
		//    while (kReader.Read())
		//    {
		//        if (kReader.NodeType == XmlNodeType.Element)
		//        {
		//            // 최상위 노드가 <EFFECT> 혹은 <SOUND>가 아니면 종료
		//            if (kReader.Name != "EFFECT" && kReader.Name != "SOUND")
		//            {
		//                kReader.Close();
		//                return false;
		//            }
		//            else
		//            {
		//                break;
		//            }
		//        }
		//    }

		//    while (kReader.Read())
		//    {
		//        if (kReader.NodeType == XmlNodeType.Element && kReader.Name == "ITEM")
		//        {
		//            if (kReader.MoveToAttribute("ID"))
		//            {
		//                string kID = kReader.Value.ToLower();
		//                if(kReader.Read() && kReader.NodeType == XmlNodeType.Text)
		//                {
		//                    string kPath = kReader.Value.ToLower();
		//                    m_mapEventList.Add(kID, kPath);
		//                }
		//            }
		//        }
		//    }

		//    kReader.Close();

		//    return true;
		//}

		//static public bool GetEventPathByID(string kID, ref string kPath)
		//{
		//    if (!m_mapEventList.Contains(kID))
		//    {
		//        return false;
		//    }

		//    string kRetPath = (string)EventList.m_mapEventList[kID];
		//    if (kRetPath == null)
		//    {
		//        return false;
		//    }
		//    kPath = kRetPath;
		//    return true;
		//}

		//static public ArrayList GetEventKeys(string kPrefix)
		//{
		//    if (m_mapEventList.Count == 0)
		//    {
		//        return null;
		//    }

		//    ArrayList kEventsList = new ArrayList(m_mapEventList.Count);
		//    foreach (DictionaryEntry eEntry in m_mapEventList)
		//    {
		//        if (eEntry.Key.ToString().Contains(kPrefix))
		//        {
		//            kEventsList.Add(eEntry);
		//        }
		//    }
		//    return kEventsList;
		//}

		//static public void ClearAllEventItems()
		//{
		//    m_mapEventList.Clear();
		//    m_mapParsingLog.Clear();
		//}
	}
}
