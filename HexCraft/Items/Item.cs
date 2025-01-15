using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public enum WeaponTypes { Axe, Pickaxe, Knife, Sword, Bow, Bucket }
public enum ItemTypes { Resource, Weapon, Armor, Crafting, Bucket, None }
[System.Serializable]
public class Item
{
    [SerializeField] string name;
    [SerializeField] int id = -1;
    [SerializeField] ItemTypes itemType;
    [SerializeField] bool isCraftable = true;
    [SerializeField] int expToGive;
    [SerializeField] bool isStackable = true;
    [SerializeField] GameObject prefab = null;
    [SerializeField] Sprite icon;
    [SerializeField] TOOLTYPE toolType;
    [SerializeField] int num0 = 1; //armor or attack
    [SerializeField] int num1 = 0;
    [SerializeField] List<InventoryItem> craftingRecipe = new List<InventoryItem>();
    [SerializeField] CraftingMenuUI.CraftingTypes craftingType = CraftingMenuUI.CraftingTypes.Tools;

    public string Name { get { return name; } set { name = value; } }
    public int Id { get { return id; } set { id = value; } }
    public ItemTypes ItemType { get { return itemType; } set { itemType = value; } }
    public bool Craftable { get { return isCraftable; } set { isCraftable = value; } }
    public int ExpToGive { get { return expToGive; } set { expToGive = value; } }
    public bool Stackable { get { return isStackable; } set { isStackable = value; } }
    public GameObject Prefab { get { return prefab; } set { prefab = value; } }
    public Sprite Icon { get { return icon; } set { icon = value; } }
    public TOOLTYPE ToolType { get { return toolType; } set { toolType = value; } }
    public int Num0 { get { return num0; } set { num0 = value; } }
    public int Num1 { get { return num1; } set { num1 = value; } }
    public List<InventoryItem> CraftingRecipe { get { return craftingRecipe; } set { craftingRecipe = value; } }
    public CraftingMenuUI.CraftingTypes CraftingType { get { return craftingType; } set { craftingType = value; } }

    public virtual void RightClick()
    {
        Debug.Log("Right Clicked");
    }

}

public enum TOOLTYPE { None, Axe, Pickax, Knife, Sword, Bucket, Length }
[System.Serializable]
public class ItemProperty
{
    public bool Breakable = true;
    public TOOLTYPE ToolType;
    public float NonToolSpeed = 1;
    public float ToolSpeed = 2;

    public ItemProperty(bool breakable, TOOLTYPE toolType, float nonSpeed, float speed)
    {
        this.Breakable = breakable;
        this.ToolType = toolType;
        this.NonToolSpeed = nonSpeed;
        this.ToolSpeed = speed;
    }
}
