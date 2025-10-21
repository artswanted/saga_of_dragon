/*///////////////////////////////////////////////////
 * FileChangeNotify 09.03.04 소스 날아가서 다시 작성
 * 지정된 패스에서 지정된  확장자의 파일이 변경되면 알려준다.
 * 한가지 변경에 대해 두가지 이상의 변경메세지가 날아오는데
 * 예로 파일 수정의 경우 변경날짜, 엑세스날짜가 변경되어
 * 나타나는 현상. 이걸 한번으로 필터링하게 처리해줌.
 * /////////////////////////////////////////////////*/

using System;
using System.Collections.Generic;
using System.Text;
using System.IO;
using System.Windows.Forms;

namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	class FileChangeNotify
	{
		private FileSystemWatcher	m_kWatcher = new FileSystemWatcher();
		private String m_kTargetPath;
		private String m_kTargetExtend;
		private NotifyFilters m_kTargetFilter;
		private bool m_bIncludeSubDir;

		//외부에서 연결할 핸들러
		public delegate void OuterHandler(object kSender, FileSystemEventArgs kEvent, RenamedEventArgs kRenameEvent);
		public OuterHandler PfnOuterHandler = null;
		
		//파일변경 메세지 중복시 걸러내기 위한 변수
		private String m_kLastChangedFileName; //마지막으로 변경된 변수
		private DateTime m_kLastWriteTime;          //마지막으로 내용이 변경된 날짜와 시간

		public OuterHandler ChangedHandler
		{
			set
			{
				PfnOuterHandler = value;
			}
		}

		private bool IsOverlappedLastWriteTime(object kSender, String kFullPath)
		{
			FileInfo kFi = new FileInfo(kFullPath);
			if ((m_kLastChangedFileName == kFullPath) && (kFi.LastWriteTime == m_kLastWriteTime))
			{
				return true;
			}
			else if ((m_kLastChangedFileName == kFullPath) && (kFi.LastWriteTime != m_kLastWriteTime))
			{
				m_kLastWriteTime = kFi.LastWriteTime;
				return false;
			}
			else
			{
				m_kLastWriteTime = kFi.LastWriteTime;
				m_kLastChangedFileName = kFullPath;
				return false;
			}
		}

		private void fcn_rename(object kSender, RenamedEventArgs kEvent)
		{
			if (null != PfnOuterHandler)
			{
				PfnOuterHandler(kSender, null, kEvent);
			} 
		}
		private void fcn_changed(object kSender, FileSystemEventArgs kEvent)
		{
			if (false == IsOverlappedLastWriteTime(kSender, kEvent.FullPath))
			{
				if (null != PfnOuterHandler)
				{
					PfnOuterHandler(kSender, kEvent, null);
				}
			}			
		}

		private void fcn_deleted(object kSender, FileSystemEventArgs kEvent)
		{
			if (null != PfnOuterHandler)
			{
				PfnOuterHandler(kSender, kEvent, null);
			}
		}
		private void fcn_created(object kSender, FileSystemEventArgs kEvent)
		{
			if (null != PfnOuterHandler)
			{
				PfnOuterHandler(kSender, kEvent, null);
			} 
		}

		public string GetChangedFileFullPath()
		{
			return m_kLastChangedFileName;
		}

		public FileChangeNotify(String kPath, String kExtend, NotifyFilters kFilter, bool bIncludeSubDir)
		{
			m_kTargetPath = kPath;
			m_kTargetExtend = kExtend;
			m_kTargetFilter = kFilter;
			m_bIncludeSubDir = bIncludeSubDir;

			m_kWatcher.Path = kPath;
			m_kWatcher.Filter = kExtend;
			m_kWatcher.NotifyFilter = kFilter;
			m_kWatcher.IncludeSubdirectories = bIncludeSubDir;

			m_kWatcher.Renamed += new RenamedEventHandler(fcn_rename);
			m_kWatcher.Changed += new FileSystemEventHandler(fcn_changed);
			m_kWatcher.Deleted += new FileSystemEventHandler(fcn_deleted);
			m_kWatcher.Created += new FileSystemEventHandler(fcn_created);
		}

		public void Start()
		{
			m_kWatcher.EnableRaisingEvents = true;
		}

		public void Stop()
		{
			m_kWatcher.EnableRaisingEvents = false;
		}
	}
}
