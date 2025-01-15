using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;

public class InventoryUI : MonoBehaviour, IMenu
{
    //public GameObject itemSlotsPrefab;
    public Image[] slots;
    public Image[] handSlots;
    public Image inventoryDragHolder;

    bool firstTime = true;

    // Start is called before the first frame update
    void Start()
    {
        //slots = new Image[Player.numSlots];
        //handSlots = new Image[Player.numHandSlots];
        StartUI();
    }

    public void StartUI()
    {
        for (int i = 0; i < slots.Length; i++)
        {
            slots[i].GetComponent<InventorySlotUI>().Init(i, GameManager.playerInstance.inventory.GetByIndex(i), GameManager.playerInstance.inventory.GetAmountByIndex(i));
        }
        for (int i = 0; i < handSlots.Length; i++)
        {
            handSlots[i].GetComponent<InventorySlotUI>().Init(i + Player.numSlots, GameManager.playerInstance.inventory.GetByIndex(i + Player.numSlots), GameManager.playerInstance.inventory.GetAmountByIndex(i + Player.numSlots));
        }
        UIManager.instance.UpdateHandSlots();
    }

    public void UpdateUI()
    {
        for (int i = 0; i < slots.Length; i++)
        {
            slots[i].GetComponent<InventorySlotUI>().UpdateUI(GameManager.playerInstance.inventory.GetByIndex(i), GameManager.playerInstance.inventory.GetAmountByIndex(i));
        }
        for (int i = 0; i < handSlots.Length; i++)
        {
            handSlots[i].GetComponent<InventorySlotUI>().UpdateUI(GameManager.playerInstance.inventory.GetByIndex(i + Player.numSlots), GameManager.playerInstance.inventory.GetAmountByIndex(i + Player.numSlots));//, this);
        }
        UIManager.instance.UpdateHandSlots();
    }

    public static InventorySlotUI IsPointerOverUIObject()
    {
        PointerEventData eventDataCurrentPosition = new PointerEventData(EventSystem.current);
        eventDataCurrentPosition.position = new Vector2(Input.mousePosition.x, Input.mousePosition.y);
        List<RaycastResult> results = new List<RaycastResult>();
        EventSystem.current.RaycastAll(eventDataCurrentPosition, results);
        for (int i = 0; i < results.Count; i++)
        {
            InventorySlotUI invSlot = results[i].gameObject.GetComponent<InventorySlotUI>();
            if(invSlot != null)
            {
                return invSlot;
            }
        }
        return null;
    }

    private void Update()
    {
    }

    public void Open()
    {
        gameObject.SetActive(true);
        if(firstTime)
        {
            StartUI();
            firstTime = false;
        }
        UpdateUI();
        UIManager.instance.showInventory = true;
    }

    public void Close()
    {
        for (int i = 0; i < slots.Length; i++)
        {
            slots[i].GetComponent<InventorySlotUI>().Reset();
        }
        for (int i = 0; i < handSlots.Length; i++)
        {
            handSlots[i].GetComponent<InventorySlotUI>().Reset();
        }
        inventoryDragHolder.gameObject.SetActive(false);
        UIManager.instance.ItemDetailsPanel.gameObject.SetActive(false);
        UIManager.instance.ItemDetailsPanel.nameText.SetText("");
        gameObject.SetActive(false);
        UIManager.instance.showInventory = false;
    }
}
