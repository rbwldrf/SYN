export var adapter = await navigator.gpu.requestAdapter();

export async function init(){
    console.log(adapter!=null);
}