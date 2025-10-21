using System;
using System.Collections;
using System.Diagnostics;
using System.Text;
using System.Windows.Forms;
using Emergent.Gamebryo.SceneDesigner.PluginAPI;
using Emergent.Gamebryo.SceneDesigner.PluginAPI.StandardServices;

namespace Emergent.Gamebryo.SceneDesigner.GUI.PluginCore.StandardServices
{
	/// <summary>
	/// Summary description for ShortCutServiceImpl.
	/// </summary>
	public class ShortcutServiceImpl : StandardServiceBase ,IShortcutService
	{

		#region Private Data
		ArrayList m_bindings;
		#endregion

		public ShortcutServiceImpl()
		{
			m_bindings = new ArrayList();
		}

		private class KeyBinding
		{
			public Keys Key;
			public UICommand Command;
		}


		public void RegisterShortcut(Keys key, UICommand pmCommmand)
		{
			KeyBinding binding = new KeyBinding();
			binding.Key = key;
			binding.Command = pmCommmand;
			m_bindings.Add(binding);
		}

		public string GetTextRepresentation(Keys key)
		{
			StringBuilder sb = new StringBuilder();
			if ((key & Keys.Control) == Keys.Control)
			{
				sb.Append("Ctrl+");
			}
			if ((key & Keys.Alt) == Keys.Alt)
			{
				sb.Append("Alt+");
			}
			if ((key & Keys.Shift) == Keys.Shift)
			{
				sb.Append("Shift+");
			}

			Keys basicKey = key & (Keys.KeyCode);
			sb.Append(basicKey.ToString());
			return sb.ToString();
		}

		public Keys GetKey(UICommand pmCommand)
		{
			foreach (KeyBinding binding in m_bindings)
			{
				if (binding.Command == pmCommand)
				{
					return binding.Key;
				}
			}
			return Keys.None;
		}

		internal bool HandleKey(Keys key)
		{
			KeyBinding binding = GetBinding(key);
			if (binding != null)
			{
				UIState state = new UIState();
				binding.Command.ValidateCommand(state);
				if (state.Enabled)
				{
					binding.Command.DoClick(this, null);
					return true;
				}
			}
			return false;
		}

		private KeyBinding GetBinding(Keys key)
		{
			foreach (KeyBinding binding in m_bindings)
			{
				if (binding.Key == key)
				{
					return binding;
				}
			}
			return null;
		}
	}
}
