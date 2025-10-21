using System;
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Text;
using System.Windows.Forms;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.GUI.ProjectG
{
	public partial class MergePanel : Form
	{
	
		//public
		public MergePanel()
		{
			InitializeComponent();
		}

		public MergePanel(string strFilepath)
		{
			//m_pmScene.Dispose();
			m_strGsaFilePath = strFilepath;
			InitializeComponent();
			ParsingGSA();
			m_tbNewSelectionSet.Hide();
			//m_tbNewSelectionSet
		}
	
		//private
		private void ParsingGSA()
		{
			m_pmScene = MFramework.Instance.Merge.ParsingGSA(m_strGsaFilePath);
			MEntity[] amEntities = m_pmScene.GetEntities();
			MSelectionSet[] amSets = m_pmScene.GetSelectionSets();

			foreach(MEntity pmEntity in amEntities)
			{//모든 엔티티에 대해
				//엔티티의 프로퍼티네임들을 얻어옴
				//String[] amPropertyNames = pmEntity.GetPropertyNames();
				//bool bIsRemoved = false;
				//foreach (String pmPropName in amPropertyNames)
				//{//모든 프로퍼티네임들에 대해
				//    if ("Light Type" == pmPropName)
				//    {//라이트 타입이면
				//        //씬에서 제거
				//        m_pmScene.RemoveEntity(pmEntity, false);
				//        foreach (MSelectionSet pmSets in amSets)
				//        {//모든 셀렉션셋들에 대해
				//            if (null != pmSets.GetEntityByName(pmEntity.Name))
				//            {//셋이 해당 엔티티를 가지고 있으면
				//                //셋에서 해당 엔티티 제거
				//                pmSets.RemoveEntity(pmEntity);
				//            }
				//        }
				//        bIsRemoved = true;
				//        break;
				//    }
				//}

				//if (false == bIsRemoved)
				{//이번 루프에서 제거된 엔티티가 아니면
					//PG프로퍼티 타입을 가져옴
					String kPropType = MFramework.Instance.Merge.ParsingPGPropertyType(pmEntity);
					if ("" != kPropType)
					{//PG프로퍼티 타입이 존재하면
						//엔티티의 프로퍼티 타입을 제대로 셋팅
						pmEntity.PGProperty = (MEntity.ePGProperty)Enum.Parse(
							typeof(MEntity.ePGProperty),
							kPropType);
					}
				}
			}

			BuildEntryList(m_pmScene);
			BuildSelectionSetList(m_pmScene);
		}

		//엔티티 리스트를 만든다.
		private void BuildEntryList(MScene pmScene)
		{
			MEntity[] amEntities = pmScene.GetEntities();
			uint iEntityCount = pmScene.EntityCount;

			m_pmEntityList.Clear();

			foreach (MEntity pmEntity in amEntities)
			{//모든 엔티티에 대해
				bool bAdd = true;

				if ((m_eFilterState == FilterState.Hidden &&
						!pmEntity.Hidden) ||
					(m_eFilterState == FilterState.Frozen &&
						!pmEntity.Frozen) ||
					(m_eFilterState == FilterState.Visible &&
						(pmEntity.Hidden || pmEntity.Frozen)))
				{//히든, 프로즌 이면...
					bAdd = false;
				}

				if (bAdd)
				{ //
					m_pmEntityList.Add(pmEntity);
				}
			}

			m_pmEntityList.Sort(m_pmEntityComparer);

			m_lbEntityList.BeginUpdate();
			int iTopIndex = m_lbEntityList.TopIndex;
			m_lbEntityList.Items.Clear();
			foreach (object pmObject in m_pmEntityList)
			{
				m_lbEntityList.Items.Add(pmObject);
			}
			m_lbEntityList.TopIndex = iTopIndex;
			m_lbEntityList.EndUpdate();
		}

		//셀렉션셋 리스트를 만든다.
		private void BuildSelectionSetList(MScene pmScene)
		{
			MSelectionSet[] amSelectionSet = pmScene.GetSelectionSets();
			uint iSelectionSetCount = pmScene.SelectionSetCount;

			m_pmSelectionSetList.Clear();

			foreach (MSelectionSet pmSelectionSet in amSelectionSet)
			{
				m_pmSelectionSetList.Add(pmSelectionSet);
			}

			m_pmSelectionSetList.Sort(m_pmSelectionSetComparer);

			m_lbSelectionSetList.BeginUpdate();
			int iTopindex = m_lblSelectionSetList.TabIndex;
			m_lbSelectionSetList.Items.Clear();
			foreach(object pmObject in m_pmSelectionSetList)
			{
				m_lbSelectionSetList.Items.Add(pmObject);
			}
			m_lbSelectionSetList.TopIndex = iTopindex;
			m_lbSelectionSetList.EndUpdate();
		}

		private void m_btnMerge_Click(object sender, EventArgs e)
		{
			string kNewSelSetName = null;
			MSelectionSet kNewSelSet = null;

			if(true==m_bGetByNewSelSetName)
			{//새 셀렉션셋 추가 기능이 켜져 있으면
				kNewSelSetName = m_tbNewSelectionSet.Text;
				kNewSelSet = MSelectionSet.Create(kNewSelSetName);
				bool bIsAllSpace = true;
				int iNameIndex = 0;
				for (iNameIndex = 0; iNameIndex < kNewSelSetName.Length; ++iNameIndex)
				{//새 셀렉션 셋이름으로 넣은 문자열이
					//모두 공백인지 체크
					bIsAllSpace &= (' ' == kNewSelSetName[iNameIndex]);
					if (false == bIsAllSpace)
						break;
				}
				if ((null == kNewSelSetName) || (true==bIsAllSpace))
				{
					MessageBox.Show("SelectionSet이름을 정해야 합니다.", "오류");
					return;
				}
				//앞의 쓸데없는 공백들을 다 잘라냄
				kNewSelSetName = kNewSelSetName.Substring(iNameIndex);
			}

			for (int iEntityIndex = 0; iEntityIndex < m_lbEntityList.Items.Count; ++iEntityIndex)
			{//모든 엔티티들 중에...
				if (true == m_lbEntityList.GetSelected(iEntityIndex))
				{//선택된 엔티티는...
					if (null != MFramework.Instance.Scene.GetEntityByName((m_lbEntityList.Items[iEntityIndex] as MEntity).Name))
					{//같은 이름을 가진 엔티티가 존재하는지 보고나서
						//있으면 이름을 바꾸고
						do
						{
							string kTempNameStr = m_pmScene.GetUniqueEntityName((m_lbEntityList.Items[iEntityIndex] as MEntity).Name + "_i");
							//while(null != m_pmScene.GetEntityByName((m_lbEntityList.Items[iEntityIndex] as MEntity).Name + "_i"))
							{
								(m_lbEntityList.Items[iEntityIndex] as MEntity).Name = kTempNameStr;
							}
							
						} while (null != MFramework.Instance.Scene.GetEntityByName((m_lbEntityList.Items[iEntityIndex] as MEntity).Name));
					}
					//추가
					MFramework.Instance.Scene.AddEntity(m_lbEntityList.Items[iEntityIndex] as MEntity, true);
					//새 셀렉션셋 기능이 켜져있으면 새 셀렉션셋에도 엔티티 추가
					if (true == m_bGetByNewSelSetName)
					{
						kNewSelSet.Name = kNewSelSetName;
						kNewSelSet.AddEntity(m_lbEntityList.Items[iEntityIndex] as MEntity);
					}
				}
			}

			if (false == m_bGetByNewSelSetName)
			{//새 그룹으로 추가 기능이 꺼져있으면
				for (int iSelSetIndex = 0; iSelSetIndex < m_lbSelectionSetList.Items.Count; ++iSelSetIndex)
				{//모든 셀렉션 셋중에..
					if (true == m_lbSelectionSetList.GetSelected(iSelSetIndex))
					{//선택된 셀렉션셋에 대해
						for (int iEntityIndex = 0; iEntityIndex < m_lbEntityList.Items.Count; ++iEntityIndex)
						{//모든 엔티티들에 대해
							if (null != (m_lbSelectionSetList.Items[iSelSetIndex] as MSelectionSet).GetEntityByName(
								(m_lbEntityList.Items[iEntityIndex] as MEntity).Name))
							{//해당 엔티티가 셀렉션셋에 있을 때
								if (false == m_lbEntityList.GetSelected(iEntityIndex))
								{//그 엔티티가 선택된 상태가 아니라면
									//셀렉션 셋에서 제거
									(m_lbSelectionSetList.Items[iSelSetIndex] as MSelectionSet).RemoveEntity(
										m_lbEntityList.Items[iEntityIndex] as MEntity);
								}
							}
						}
						if (null != MFramework.Instance.Scene.GetSelectionSetByName((
							m_lbSelectionSetList.Items[iSelSetIndex] as MSelectionSet).Name))
						{//같은 이름을 가진 셀렉션 셋이 존재하는지 보고
							//있으면 이름을 바꿈
							do
							{
								string kTempNameStr = m_pmScene.GetUniqueSelectionSetName((m_lbSelectionSetList.Items[iSelSetIndex] as MSelectionSet).Name + "_i");
								(m_lbSelectionSetList.Items[iSelSetIndex] as MSelectionSet).Name = kTempNameStr;
							} while (null != MFramework.Instance.Scene.GetSelectionSetByName((m_lbSelectionSetList.Items[iSelSetIndex] as MSelectionSet).Name));
							
						}
						if (1 <= (m_lbSelectionSetList.Items[iSelSetIndex] as MSelectionSet).EntityCount)
						{//위에서 처리가 끝난 셀렉션 셋에 엔티티가 하나이상 있으면
							//추가
							MFramework.Instance.Scene.AddSelectionSet(m_lbSelectionSetList.Items[iSelSetIndex] as MSelectionSet);
						}
					}
				}
			}
			else
			{//새 셀렉션셋 추가 기능이 켜져 있으면
				if (null != MFramework.Instance.Scene.GetSelectionSetByName(kNewSelSetName))
				{//새 셀렉션셋이름이 이미 존재하는지 보고
					//존재하면 이름을 바꿈
					do
					{
						string kTempNameStr = m_pmScene.GetUniqueSelectionSetName(kNewSelSet.Name + "_i");
						kNewSelSet.Name = kTempNameStr;
					} while (null != MFramework.Instance.Scene.GetSelectionSetByName(kNewSelSet.Name));
				}
				//새 셋 추가
				if(1<=kNewSelSet.EntityCount)
				{
					MFramework.Instance.Scene.AddSelectionSet(kNewSelSet);
				}
			}

			this.Close();
		}

		private void m_btnCancel_Click(object sender, EventArgs e)
		{
			this.Close();
		}

		#region EntityComparer class
		private class EntityComparer : IComparer
		{
			#region IComparer Members
			public int Compare(object pmObjectA, object pmObjectB)
			{
				MEntity pmEntityA = pmObjectA as MEntity;
				MEntity pmEntityB = pmObjectB as MEntity;
				if (pmEntityA == null || pmEntityB == null)
				{
					throw new ArgumentException("EntityComparer only " +
						"supports comparing MEntity instances.");
				}

				return pmEntityA.Name.CompareTo(pmEntityB.Name);
			}
			#endregion
		}
		#endregion

		#region SelectionSetComparer class
		private class SelectionSetComparer : IComparer
		{
			#region IComparer Members
			public int Compare(object pmObjectA, object pmObjectB)
			{
				MSelectionSet pmSelctionSetA = pmObjectA as MSelectionSet;
				MSelectionSet pmSelctionSetB = pmObjectB as MSelectionSet;
				if (pmSelctionSetA == null || pmSelctionSetB == null)
				{
					throw new ArgumentException("EntityComparer only " +
						"supports comparing MEntity instances.");
				}

				return pmSelctionSetA.Name.CompareTo(pmSelctionSetB.Name);
			}
			#endregion
		}
		#endregion

		//SelectionSetList에서 선택된 것들과 EntriList의 것들을 매칭시킴
		private void m_lbSelectionSetList_SelectedIndexChanged(object sender, EventArgs e)
		{
			MSelectionSet[] kSelectoinSet = m_pmScene.GetSelectionSets();
			MSelectionSet kSelectedSet = null;
			m_lbEntityList.ClearSelected();
			
			for (int iSelectionSetIndex = 0; iSelectionSetIndex < kSelectoinSet.Length; ++iSelectionSetIndex)
			{
				if (true == m_lbSelectionSetList.GetSelected(iSelectionSetIndex))
				{
					kSelectedSet = m_lbSelectionSetList.Items[iSelectionSetIndex] as MSelectionSet;
					MEntity[] kEntities = kSelectedSet.GetEntities();

					m_lbEntityList.BeginUpdate();
					for(int iListIndex=0 ; iListIndex < m_lbEntityList.Items.Count ; ++iListIndex)
					{//엔트리 리스트박스에 들어있는 모든 요소에 대해
						for (int iEntityIndex = 0; iEntityIndex < kEntities.Length; ++iEntityIndex)
						{//엔티티 배열의 모든 요소에 대해
							//위 엔트리 배열[iEntityIndex].Name과 리스트박스요소[iListIndex].name이 같은가?
							if (kEntities[iEntityIndex].Name == (m_lbEntityList.Items[iListIndex] as MEntity).Name)
							{//같으면 리스트박스요소[iListIndex]를 선택상태로...
								m_lbEntityList.SetSelected(iListIndex, true);
							}							
						}						
					}
					m_lbEntityList.EndUpdate();
				}
			}
		}

		private void m_lbEntityList_SelectedIndexChanged(object sender, EventArgs e)
		{
			m_iSectectedEntityCount = 0;
			for (int iIndex = 0; iIndex < m_lbEntityList.Items.Count; ++iIndex)
			{
				if (m_lbEntityList.GetSelected(iIndex))
				{
					++m_iSectectedEntityCount;
				}
			}
		}

		private void m_cbGetByNewSelectionSet_CheckedChanged(object sender, EventArgs e)
		{
			m_bGetByNewSelSetName = !m_bGetByNewSelSetName;
			if (true == m_bGetByNewSelSetName)
			{
				m_tbNewSelectionSet.Show();
			}
			else
			{
				m_tbNewSelectionSet.Hide();
			}
		}
	}
}