using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

public class HexagonEditor : BaseEditor
{
    public ItemDatabase itemDatabase;
    HexagonDatabase database;
    HexagonType tempHexagon;

    List<string> itemsNames = new List<string>();
    List<string> hexagonTypes = new List<string>();

    [MenuItem("Custom/Hexagon Editor")]
    public static void Init()
    {
        HexagonEditor window = GetWindow<HexagonEditor>();
        window.minSize = new Vector2(800, 600);
        window.titleContent.text = "Hexagon Editor";
        window.Show();
    }

    protected override void OnEnable()
    {
        base.OnEnable();
        UpdateList();
    }

    protected override void UpdateList()
    {
        base.UpdateList();
        itemsNames = new List<string>();
        hexagonTypes = new List<string>();
        itemDatabase = Resources.Load("Databases/ItemDatabase") as ItemDatabase;
        database = Resources.Load("Databases/HexagonDatabase") as HexagonDatabase;
        for (int i = 0; i < itemDatabase.Count; i++)
        {
            itemsNames.Add(itemDatabase.database[i].Name);
        }
        for (int i = 0; i < (int)HexagonTypes.None; i++)
        {
            hexagonTypes.Add(((HexagonTypes)i).ToString());
        }
    }

    protected override void OnFocus()
    {
        base.OnFocus();
    }

    protected override void OnGUI()
    {
        base.OnGUI();
    }

    protected override void ListView()
    {
        base.ListView();
    }

    bool variationDropdown = false;
    List<bool> showVariation = new List<bool>();
    List<bool> showResources = new List<bool>();
    protected override void ItemDetails()
    {
        base.ItemDetails();
        GUILayout.BeginVertical("Box", GUILayout.ExpandWidth(true), GUILayout.ExpandHeight(true));
        GUILayout.BeginVertical(GUILayout.ExpandWidth(true), GUILayout.ExpandHeight(true));

        if (showDetails)
        {
            GUILayout.BeginVertical();

            //ID
            GUILayout.Label("ID: " + tempHexagon.id);

            //Hexagon type
            int hexagonIndex = (int)tempHexagon.hexagonType;
            hexagonIndex = EditorGUILayout.Popup("Hexagon Type: ", hexagonIndex, hexagonTypes.ToArray());
            tempHexagon.hexagonType = (HexagonTypes)hexagonIndex;

            //is craftable
            tempHexagon.isCraftable = EditorGUILayout.Toggle("Is Craftable", tempHexagon.isCraftable);

            GUILayout.Space(10);

            //variations
            variationDropdown = EditorGUILayout.Foldout(variationDropdown, "Variations", toggleOnLabelClick: true);
            GUILayout.BeginVertical("box");
            if (variationDropdown)
            {
                for (int i = 0; i < tempHexagon.hexagonVariations.Count; i++)
                {
                    //delete and toggle dropdown
                    GUILayout.BeginHorizontal();
                    if (GUILayout.Button("X", GUILayout.Width(30)))
                    {
                        tempHexagon.hexagonVariations.RemoveAt(i);
                        showVariation.RemoveAt(i);
                    }
                    showVariation[i] = EditorGUILayout.Foldout(showVariation[i], "Variation " + (i + 1), toggleOnLabelClick: true);
                    GUILayout.EndHorizontal();

                    //show each variation
                    if (showVariation[i])
                    {
                        //exp to give and prefab
                        GUILayout.BeginVertical("box");
                        tempHexagon.hexagonVariations[i].expToGive = EditorGUILayout.IntField("Exp To Give: ", tempHexagon.hexagonVariations[i].expToGive);
                        tempHexagon.hexagonVariations[i].hexagonPrefab = EditorGUILayout.ObjectField("Prefab", tempHexagon.hexagonVariations[i].hexagonPrefab, typeof(GameObject), false) as GameObject;
                        GUILayout.EndVertical();

                        //resources
                        GUILayout.BeginVertical();
                        for (int y = 0; y < tempHexagon.hexagonVariations[i].resources.Count; y++)
                        {
                            //delete and toggle dropdown
                            GUILayout.BeginHorizontal();
                            if (GUILayout.Button("X", GUILayout.Width(30)))
                            {
                                tempHexagon.hexagonVariations[i].resources.RemoveAt(y);
                                showResources.RemoveAt(i);
                            }
                            string name = itemDatabase.GetByID(tempHexagon.hexagonVariations[i].resources[y].id).Name;
                            showResources[y] = EditorGUILayout.Foldout(showResources[y], name, toggleOnLabelClick: true);
                            GUILayout.EndHorizontal();

                            if(showResources[y])
                            {
                                int index = itemDatabase.GetIndexByID(tempHexagon.hexagonVariations[i].resources[y].id);
                                GUILayout.BeginVertical();
                                index = EditorGUILayout.Popup(index, itemsNames.ToArray());
                                tempHexagon.hexagonVariations[i].resources[y].id = itemDatabase.GetIDByIndex(index);

                                tempHexagon.hexagonVariations[i].resources[y].amount = EditorGUILayout.IntField("Amount: ", tempHexagon.hexagonVariations[i].resources[y].amount);
                                GUILayout.EndVertical();
                            }
                        }

                        if (GUILayout.Button("Add Resource"))
                        {
                            tempHexagon.hexagonVariations[i].resources.Add(new InventoryItem(0,0));
                            showResources.Add(false);
                        }
                        GUILayout.EndVertical();
                    }
                }
                if(GUILayout.Button("Add Variation"))
                {
                    tempHexagon.hexagonVariations.Add(new HexagonVariation());
                    showVariation.Add(false);
                }
            }
            GUILayout.EndVertical();
            GUILayout.EndVertical();
        }


        GUILayout.EndVertical();
        GUILayout.Space(50);

        GUILayout.EndVertical();
    }

    protected override void CallViewList()
    {
        base.CallViewList();
        HexagonType hexagon;
        for (int i = 0; i < database.Count; i++)
        {
            hexagon = database.GetByIndex(i);
            GUILayout.BeginHorizontal("Box", GUILayout.Width(225));
            if (!editing)
            {
                if (GUILayout.Button("Edit", GUILayout.Width(40)))
                {
                    tempHexagon = hexagon;
                    showVariation = new List<bool>();
                    showResources = new List<bool>();
                    for (int y = 0; y < tempHexagon.hexagonVariations.Count; y++)
                    {
                        showVariation.Add(false);
                        for (int z = 0; z < tempHexagon.hexagonVariations[y].resources.Count; z++)
                        {
                            showResources.Add(false);
                        }
                    }
                    index = i;
                    showDetails = true;
                    editing = true;
                }
            }
            else
            {
                if (GUILayout.Button("Edit", GUILayout.Width(40)))
                {
                    EditorUtility.DisplayDialog("Editing", "Finish Editing Current Selection.", "Ok");
                }
            }

            GUILayout.Label(hexagon.id + " - " + hexagon.hexagonType.ToString(), GUILayout.Width(130));
            GUILayout.EndHorizontal();
        }
    }

    protected override void DisplayButtons()
    {
        base.DisplayButtons();
        if (!showDetails)
        {
            if (GUILayout.Button("Create New"))
            {
                tempHexagon = new HexagonType();
                tempHexagon.id = database.currentId;
                showDetails = true;
            }
        }
        else
        {
            if (GUILayout.Button("Save and Return"))
            {
                if (editing)
                {
                    showDetails = false;
                    database.Swap(tempHexagon, index);
                    tempHexagon = null;
                    editing = false;
                }
                else
                {
                    showDetails = false;
                    database.Add(tempHexagon);
                    tempHexagon = null;
                    editing = false;
                }
            }
            if (GUILayout.Button("Save and Create Another"))
            {
                if (editing)
                {
                    showDetails = true;
                    database.Swap(tempHexagon, index);
                    editing = false;
                    tempHexagon = new HexagonType();
                    tempHexagon.id = database.currentId;
                }
                else
                {
                    showDetails = true;
                    database.Add(tempHexagon);
                    tempHexagon = new HexagonType();
                    tempHexagon.id = database.currentId;
                }
            }
            if (editing)
            {
                if (GUILayout.Button("Delete"))
                {
                    if (EditorUtility.DisplayDialog("Confirm Delete", "Are you sure you want to delete this?", "Yes", "Cancel"))
                    {
                        database.Delete(index);
                        showDetails = false;
                        editing = false;
                        tempHexagon = null;
                    }
                }
            }
            if (GUILayout.Button("Cancel"))
            {
                showDetails = false;
                editing = false;
                tempHexagon = null;
            }
        }
    }

    protected override void BottomBar()
    {
        base.BottomBar();
    }
}
