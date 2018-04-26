function cmpPageNavigation( eleParent, numResultsPerPage )
{
    // private members
    var mResultsPerPage = numResultsPerPage;
    var mTotalPages = 0;
    var mCurrentPage = 0;
    var mTotalResults = 0;
    var mFuncChangePage = null;
    var mEleParent = eleParent;

    // public members
    var me = {};

    me.SetOnClick = function( func )
                {
                    mFuncChangePage = func;
                };

    me.RefreshUI = function()
                {
                    let firstPage = mCurrentPage - 2; if( firstPage<0 ) firstPage = 0;
                    let lastPage  = mCurrentPage + 2; if( lastPage >(mTotalPages-1) ) lastPage = mTotalPages-1;
                    let numPages = lastPage - firstPage + 1;

                    // remove all children
                    while (mEleParent.firstChild)
                    {
                        mEleParent.removeChild(mEleParent.firstChild);
                    }

                    eleButtons = document.createElement("div");
                    eleButtons.style="display:inline-block;";

                    if( firstPage != 0 )
                    {
                        let eleTmp = document.createElement("div");
                        eleTmp.id = 'pageButtonFirst';
                        eleTmp.className = pageButtons;
                        eleTmp.textContent = "1";
                        eleTmp.appendChild(button);

                        eleTmp = document.createTextNode("...");
                        eleButtons.appendChild(eleTmp);
                    }

                    for( let i=0; i<numPages; i++ )
                    {
                        let pageTo = firstPage + i;
                        let eleTmp = document.createElement("div");
                        eleTmp.id = 'pageButton' + i;
                        eleTmp.className = ( pageTo==mCurrentPage ) ? "pageButtonsCurrent" : "pageButtons";
                        eleTmp.textContent = (pageTo+1).toString();
                        eleTmp.onclick = function() { mCurrentPage=i; mFuncChangePage(i); me.RefreshUI() };
                        eleButtons.appendChild(eleTmp);
                    }

                    let pageTo = mTotalPages - 1;
                    if( firstPage + numPages != mTotalPages )
                    {
                        let eleTmp = document.createTextNode("...");
                        eleButtons.appendChild(eleTmp);

                        eleTmp = document.createElement("div");
                        eleTmp.id = 'pageButtonLast';
                        eleTmp.className = "pageButtons";
                        eleTmp.textContent = mTotalPages.toString();
                        eleButtons.appendChild(eleTmp);
                    }

                    mEleParent.appendChild(eleButtons);

                    let eleTotalText = document.createElement("div");
                    eleTotalText.style="display:inline-block;margin-left:32px;";
                    eleTotalText.textContent = "(" + mTotalResults + ")";
                    mEleParent.appendChild(eleTotalText);
                    };

    me.SetNumResults = function( numResults, numResultsPerPage )
                     {
                        mResultsPerPage = numResultsPerPage;
                        mTotalResults = numResults;
                        mTotalPages = (((numResults+mResultsPerPage-1)|0) / mResultsPerPage)|0
                        mCurrentPage = 0;
                     };

    return me;
}

