using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

public class BaseEditor : EditorWindow
{
    protected Vector2 listViewScroll;
    protected int _listViewWidth = 235;
    protected bool editing = false;
    protected bool showDetails = false;
    protected int index = 0;

    protected virtual void OnEnable() { }
    protected virtual void UpdateList() { }
    protected virtual void OnFocus()
    {
        UpdateList();
    }
    protected virtual void OnGUI()
    {
        GUILayout.BeginHorizontal();
        ListView();

        GUILayout.EndHorizontal();
        BottomBar();
    }

    protected virtual void ListView()
    {
        EditorGUILayout.BeginVertical("Box", GUILayout.Width(_listViewWidth + 10));
        listViewScroll = GUILayout.BeginScrollView(listViewScroll, false, false, GUIStyle.none, GUI.skin.verticalScrollbar, GUILayout.ExpandHeight(true), GUILayout.Width(_listViewWidth + 10));

        CallViewList();

        EditorGUILayout.EndScrollView();
        EditorGUILayout.EndVertical();
        ItemDetails();
    }
    protected virtual void ItemDetails() { }
    protected virtual void CallViewList() { }
    protected virtual void DisplayButtons() { }

    protected virtual void BottomBar()
    {
        GUILayout.BeginHorizontal("Box", GUILayout.ExpandWidth(true));
        DisplayButtons();
        GUILayout.EndHorizontal();
    }
}
