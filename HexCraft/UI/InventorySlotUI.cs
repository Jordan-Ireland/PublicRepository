using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.UI;
using TMPro;

public class InventorySlotUI : MonoBehaviour, IBeginDragHandler, IEndDragHandler, IDragHandler, IPointerEnterHandler, IPointerExitHandler
{
    public TextMeshProUGUI text;
    public Item heldItem;
    public int index;
    int amount;
    public InventoryUI inventoryUI;

    public Image dragHolder;
    Image myImage;
    float timeSinceEnter = 0;
    bool entered;

    void Update()
    {
        if (entered)
        {
            timeSinceEnter += Time.deltaTime;
            if (timeSinceEnter >= 0.5f)
            {
                UIManager.instance.ItemDetailsPanel.gameObject.SetActive(true);
                UIManager.instance.ItemDetailsPanel.nameText.SetText(heldItem.Name);
                UIManager.instance.ItemDetailsPanel.MoveToMouse();
            }
        }
        //else
        //{
        //    UIManager.instance.ItemDetailsPanel.gameObject.SetActive(false);
        //    UIManager.instance.ItemDetailsPanel.nameText.SetText("");
        //}
    }

    public void Reset()
    {
        entered = false;
        timeSinceEnter = 0;
    }

    public void OnPointerEnter(PointerEventData eventData)
    {
        if (heldItem.Id != -1)
        {
            entered = true;
        }else
        {
            entered = false;
        }
    }

    public void OnPointerExit(PointerEventData eventData)
    {
        if(entered)
        {
            timeSinceEnter = 0;
            UIManager.instance.ItemDetailsPanel.gameObject.SetActive(false);
            entered = false;
        }
    }

    public void OnBeginDrag(PointerEventData eventData)
    {
        if (heldItem.Id == -1) return;
        dragHolder.gameObject.SetActive(true);
        dragHolder.sprite = heldItem.Icon;
        dragHolder.GetComponent<Transform>().position = Input.mousePosition;
    }

    public void OnEndDrag(PointerEventData eventData)
    {
        if (heldItem.Id == -1) return;
        InventorySlotUI slot = InventoryUI.IsPointerOverUIObject();
        if (slot != null)
        {
            GameManager.playerInstance.inventory.SwitchItem(index, slot.index);
            inventoryUI.UpdateUI();
            GameManager.playerInstance.UpdateHandSlots();
            //UpdateUI(GameManager.playerInstance.inventory.GetByIndex(index), GameManager.playerInstance.inventory.GetAmountByIndex(index));
            //slot.UpdateUI(GameManager.playerInstance.inventory.GetByIndex(slot.index), GameManager.playerInstance.inventory.GetAmountByIndex(slot.index));
        }
        dragHolder.gameObject.SetActive(false);
        dragHolder.sprite = null;
    }

    public void SwitchItems(InventorySlotUI otherSlot)
    {
        Item tempItem = heldItem;
        heldItem = otherSlot.heldItem;
        otherSlot.heldItem = tempItem;        
    }

    public void OnDrag(PointerEventData eventData)
    {
        if (heldItem.Id == -1) return;
        dragHolder.GetComponent<Transform>().position = Input.mousePosition;
    }

    public void Init(int index, Item item, int amount)//, InventoryUI inventoryUI)
    {
        myImage = GetComponent<Image>();
        //this.inventoryUI = inventoryUI;
        //dragHolder = inventoryUI.inventoryDragHolder;
        this.index = index;
        UpdateUI(item, amount);
    }

    public void UpdateUI(Item item, int amount)
    {
        this.heldItem = item;
        this.amount = amount;
        if (heldItem != null)
        {
            myImage.sprite = heldItem.Icon;
        }
        if (this.amount > -1)
        {
            text.SetText(this.amount.ToString());
        }else
        {
            text.SetText("");
        }
    }
}
