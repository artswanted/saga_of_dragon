using System;
using Emergent.Gamebryo.SceneDesigner.Framework;
using Emergent.Gamebryo.SceneDesigner.GUI.PluginCore.StandardServices;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.GUI.Utility
{
	/// <summary>
	/// Summary description for DirtyBitUtilitys.
	/// </summary>
	public class DirtyBitUtilities
	{
		#region Private Data

		#endregion

		public DirtyBitUtilities()
		{
		}

		public static void Init()
		{
			
		}

		private static void OnSettingChanged(object pmSender, 
			SettingChangedEventArgs pmEventArgs)
		{
			MFramework.Instance.Scene.Dirty = true;
		}

		public static void MakeSceneClean(MScene pmScene)
		{
			pmScene.Dirty = false;
			MEntity[] entities = pmScene.GetEntities();
			foreach(MEntity entity in entities)
			{
				entity.Dirty = false;
			}
		}

	}
}
