using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[System.Serializable]
public class PlayerInventory
{
    public InventoryItem[] inventory = new InventoryItem[0];
    int numSlots, numHandSlots;

    public int Count { get { return inventory.Length; } }

    public void SwitchItem(int indexA, int indexB)
    {
        InventoryItem tempItem = inventory[indexA];
        inventory[indexA] = inventory[indexB];
        inventory[indexB] = tempItem;
    }

    public (int, InventoryItem) GetById(int id)
    {
        for (int i = 0; i < inventory.Length; i++)
        {
            if (inventory[i].id == id)
                return (i, inventory[i]);
        }
        return (-1, null);
    }

    public int GetAmountByIndex(int index)
    {
        return inventory[index].amount;
    }

    public PlayerInventory(int numSlots, int numHandSlots)
    {
        this.numSlots = numSlots;
        this.numHandSlots = numHandSlots;
        inventory = new InventoryItem[numSlots + numHandSlots];

        for (int i = 0; i < inventory.Length; i++)
        {
            inventory[i] = new InventoryItem();
        }
    }

    public Item GetByIndex(int index)
    {
        return GameManager.itemDatabase.GetByID(inventory[index].id);
    }

    public bool AddItemToInventory(int id, int amount)
    {
        Item item = GameManager.itemDatabase.GetByID(id);
        for (int i = 0; i < inventory.Length; i++)
        {
            if(inventory[i].id == id)
            {
                if(item.Stackable)
                {
                    inventory[i].AddAmount(amount);
                    UIManager.instance.UpdateHandSlots();
                    return true;
                }
            }
        }
        for (int i = 0; i < inventory.Length; i++)
        {
            if (inventory[i].id == -1)
            {
                inventory[i] = new InventoryItem(id, amount);
                UIManager.instance.UpdateHandSlots();
                return true;
            }
        }
        return false;
    }

    public void AddItemByIndex(int index, int id, int amount)
    {
        inventory[index] = new InventoryItem(id, amount);
    }

    public bool RemoveItem(int id, int amount)
    {
        for (int i = 0; i < inventory.Length; i++)
        {
            if (inventory[i].id == id)
            {
                inventory[i].AddAmount(-amount);
                if(inventory[i].amount <= 0)
                {
                    inventory[i] = new InventoryItem();
                }
                UIManager.instance.UpdateHandSlots();
                return true;
            }
        }
        return false;
    }

    public bool RemoveItems(HexagonVariation hex)
    {
        for (int i = 0; i < hex.resources.Count; i++)
        {
            int index = ContainsID(hex.resources[i].id);
            if (index != -1)
            {
                RemoveItem(hex.resources[i].id, hex.resources[i].amount);
                UIManager.instance.UpdateHandSlots();
            }
            else
            {
                return false;
            }
        }
        return true;
    }

    public void AddItemToBar(int inventoryIndex, int itemBarIndex)
    {
        InventoryItem item = inventory[itemBarIndex + 25];
        inventory[25 + itemBarIndex] = inventory[inventoryIndex];
        inventory[inventoryIndex] = item;
        UIManager.instance.UpdateHandSlots();
    }

    public bool CanCraft(HexagonVariation hexVar)
    {
        for (int i = 0; i < hexVar.resources.Count; i++)
        {
            int index = ContainsID(hexVar.resources[i].id);
            if (index != -1)
            {
                if (inventory[index].amount < hexVar.resources[i].amount)
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
        return true;
    }

    bool HasInventorySpace()
    {
        for (int i = 0; i < inventory.Length; i++)
        {
            if (inventory[i].id == -1)
            {
                return true;
            }
        }
        return false;
    }

    public bool CanCraft(List<InventoryItem> recipe)
    {
        if (!HasInventorySpace()) return false;
        for (int i = 0; i < recipe.Count; i++)
        {
            int index = ContainsID(recipe[i].id);
            if (index != -1)
            {
                if (inventory[index].amount < recipe[i].amount)
                    return false;
            }
            else
                return false;
        }
        return true;
    }

    int ContainsID(int id)
    {
        for (int i = 0; i < inventory.Length; i++)
        {
            if (inventory[i].id == id)
                return i;
        }

        return -1;
    }
}

[System.Serializable]
public class InventoryItem
{
    public int id;
    public int amount;

    public InventoryItem()
    {
        id = -1;
        amount = -1;
    }

    public InventoryItem(int id, int amount)
    {
        this.id = id;
        this.amount = amount;
    }

    public void AddAmount(int amount)
    {
        this.amount += amount;
    }
}
