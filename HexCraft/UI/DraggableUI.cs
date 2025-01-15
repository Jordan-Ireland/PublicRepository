using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;

public class DraggableUI : MonoBehaviour, IBeginDragHandler, IEndDragHandler, IDragHandler
{
    public RectTransform parentObj;
    RectTransform myRect;
    public Vector3 offset;

    public void OnBeginDrag(PointerEventData eventData)
    {
        offset.x = Input.mousePosition.x - myRect.position.x;
        offset.y = Input.mousePosition.y - parentObj.position.y;
        parentObj.position = Input.mousePosition - offset;
    }

    public void OnDrag(PointerEventData eventData)
    {
        parentObj.position = Input.mousePosition - offset;
    }

    public void OnEndDrag(PointerEventData eventData)
    {
        parentObj.position = Input.mousePosition - offset;
    }

    // Start is called before the first frame update
    void Start()
    {
        myRect = GetComponent<RectTransform>();
    }
}
