using System.Collections;
using System.Collections.Generic;
using UnityEditor;
using UnityEngine;

public class ItemEditor : BaseEditor
{
    public ItemDatabase database;
    List<string> itemTypesString = new List<string>();
    List<string> toolTypesString = new List<string>();
    List<string> craftingTypeString = new List<string>();
    List<string> itemNameString = new List<string>();
    Item tempItem;

    [MenuItem("Custom/Item Editor")]
    public static void Init()
    {
        ItemEditor window = GetWindow<ItemEditor>();
        window.minSize = new Vector2(800, 600);
        window.titleContent.text = "Item Editor";
        window.Show();
    }

    protected override void OnEnable()
    {
        base.OnEnable();
        UpdateList();
    }

    void UpdateItems()
    {
        itemNameString = new List<string>();
        for (int i = 0; i < database.Count; i++)
        {
            itemNameString.Add(database.GetByIndex(i).Name);
        }
    }

    protected override void UpdateList()
    {
        base.UpdateList();
        itemTypesString = new List<string>();
        toolTypesString = new List<string>();
        craftingTypeString = new List<string>();
        database = Resources.Load("Databases/ItemDatabase") as ItemDatabase;
        UpdateItems();
        for (int i = 0; i < (int)ItemTypes.None; i++)
        {
            itemTypesString.Add(((ItemTypes)i).ToString());
        }
        for (int i = 0; i < (int)TOOLTYPE.Length; i++)
        {
            toolTypesString.Add(((TOOLTYPE)i).ToString());
        }
        for (int i = 0; i < (int)CraftingMenuUI.CraftingTypes.Length; i++)
        {
            craftingTypeString.Add(((CraftingMenuUI.CraftingTypes)i).ToString());
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

    bool itemCraftingDropdown = false;
    protected override void ItemDetails()
    {
        base.ItemDetails();
        GUILayout.BeginVertical("Box", GUILayout.ExpandWidth(true), GUILayout.ExpandHeight(true));
        GUILayout.BeginVertical(GUILayout.ExpandWidth(true), GUILayout.ExpandHeight(true));

        if (showDetails)
        {
            GUILayout.BeginVertical();

            //ID
            GUILayout.Label("ID " + tempItem.Id);

            tempItem.Name = EditorGUILayout.TextField("Item Name", tempItem.Name);

            //Hexagon type
            int itemTypeIndex = (int)tempItem.ItemType;
            itemTypeIndex = EditorGUILayout.Popup("Item Type", itemTypeIndex, itemTypesString.ToArray());
            tempItem.ItemType = (ItemTypes)itemTypeIndex;


            int toolTypeIndex = (int)tempItem.ToolType;
            toolTypeIndex = EditorGUILayout.Popup("Tool Type", toolTypeIndex, toolTypesString.ToArray());
            tempItem.ToolType = (TOOLTYPE)toolTypeIndex;

            //is craftable
            tempItem.Craftable = EditorGUILayout.Toggle("Is Craftable", tempItem.Craftable);
            tempItem.Stackable = EditorGUILayout.Toggle("Is Stackable", tempItem.Stackable);
            tempItem.Prefab = EditorGUILayout.ObjectField("Prefab", tempItem.Prefab, typeof(GameObject), false) as GameObject;
            tempItem.Icon = EditorGUILayout.ObjectField("Icon", tempItem.Icon, typeof(Sprite), false) as Sprite;
            tempItem.Num0 = EditorGUILayout.IntField("Attack Dmg", tempItem.Num0);
            tempItem.Num1 = EditorGUILayout.IntField("Armor Amount", tempItem.Num1);

            tempItem.ExpToGive = EditorGUILayout.IntField("Exp To Give", tempItem.ExpToGive);

            GUILayout.Space(10);

            //variations
            if (tempItem.Craftable)
            {
                int craftingTypeIndex = (int)tempItem.CraftingType;
                craftingTypeIndex = EditorGUILayout.Popup("Crafting Type", craftingTypeIndex, craftingTypeString.ToArray());
                tempItem.CraftingType = (CraftingMenuUI.CraftingTypes)craftingTypeIndex;

                itemCraftingDropdown = EditorGUILayout.Foldout(itemCraftingDropdown, "Resources To Craft", toggleOnLabelClick: true);
                GUILayout.BeginVertical("box");
                if (itemCraftingDropdown)
                {
                    for (int i = 0; i < tempItem.CraftingRecipe.Count; i++)
                    {
                        GUILayout.BeginHorizontal();
                        if(GUILayout.Button("X", GUILayout.Width(25)))
                        {
                            tempItem.CraftingRecipe.RemoveAt(i);
                        }
                        if (tempItem.CraftingRecipe[i].id == -1) tempItem.CraftingRecipe[i].id = 0;
                        int itemResource = database.GetIndexByID(tempItem.CraftingRecipe[i].id);
                        itemResource = EditorGUILayout.Popup(itemResource, itemNameString.ToArray());
                        tempItem.CraftingRecipe[i].id = database.GetByIndex(itemResource).Id;

                        tempItem.CraftingRecipe[i].amount = EditorGUILayout.IntField("Amount", tempItem.CraftingRecipe[i].amount);
                        GUILayout.EndHorizontal();
                    }
                    if(GUILayout.Button("Add"))
                    {
                        tempItem.CraftingRecipe.Add(new InventoryItem());
                    }
                }
                GUILayout.EndVertical();
            }
            GUILayout.EndVertical();
        }


        GUILayout.EndVertical();
        GUILayout.Space(50);

        GUILayout.EndVertical();
    }

    protected override void CallViewList()
    {
        base.CallViewList();
        Item item;
        for (int i = 0; i < database.Count; i++)
        {
            item = database.GetByIndex(i);
            GUILayout.BeginHorizontal("Box", GUILayout.Width(225));
            if (!editing)
            {
                if (GUILayout.Button("Edit", GUILayout.Width(40)))
                {
                    tempItem = item;
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

            GUILayout.Label(item.Id + " - " + item.Name, GUILayout.Width(130));
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
                tempItem = new Item();
                tempItem.Id = database.currentId;
                showDetails = true;
            }
        }
        else
        {
            if (GUILayout.Button("Save and Return"))
            {
                showDetails = false;
                if (editing)
                {
                    database.Swap(tempItem, index);
                }
                else
                {
                    database.Add(tempItem);
                }
                tempItem = null;
                editing = false;
                UpdateItems();
            }
            if (GUILayout.Button("Save and Create Another"))
            {
                showDetails = true;
                if (editing)
                {
                    database.Swap(tempItem, index);
                    editing = false;
                }
                else
                {
                    database.Add(tempItem);
                }
                tempItem = new Item();
                tempItem.Id = database.currentId;
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
                        tempItem = null;
                    }
                }
            }
            if (GUILayout.Button("Cancel"))
            {
                showDetails = false;
                editing = false;
                tempItem = null;
            }
        }
    }

    protected override void BottomBar()
    {
        base.BottomBar();
    }
}
